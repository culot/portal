/*-
 * Copyright (c) 2016 Frederic Culot <culot@FreeBSD.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer
 *    in this position and unchanged.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR(S) ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR(S) BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <curses.h>

#include "scrollwindow.h"


namespace portal {
namespace gfx {

ScrollWindow::ScrollWindow(const Size& size, const Point& pos, const Style& style)
  : Window(size, pos, style) {
  sizePad_.setWidth(size.width() - 2);
  sizePad_.setHeight(size.height());
  createPad();
}

ScrollWindow::~ScrollWindow() {
  delwin(pad_);
}

int ScrollWindow::getCursorRowNum() const {
  return posCursor_.y();
}

void ScrollWindow::draw() const {
  Window::draw();
  applyCursorLineStyle();
  drawScrollBar();
  pnoutrefresh(pad_,
               posPad_.y(),
               posPad_.x(),
               position().y() + (style().borders ? 1 : 0),
               position().x() + 1,
               position().y() + size().height() - (style().borders ? 2 : 0),
               position().x() + size().width() - 2 - (style().borders ? 1 : 0));
}

void ScrollWindow::clear() {
  clearPrintArea();
}

void ScrollWindow::newline() {
  extendPrintArea();
  posPrint_.setY(posPrint_.y() + 1);
}

void ScrollWindow::print(const std::string& line, const Style& style) {
  int xpos;
  switch (style.align) {
  case Style::Alignment::left:
    xpos = posPad_.x();
    break;
  case Style::Alignment::center:
    xpos = (size().width() - line.length()) / 2;
    break;
  case Style::Alignment::right:
    xpos = size().width() - line.length() - 2 - (Window::style().borders ? 2 : 0);
    break;
  }

  mvwaddstr(pad_, posPrint_.y(), xpos, line.c_str());
  draw();
}

void ScrollWindow::scrollDown() {
  if (canScrollDown()) {
    posPad_.setY(posPad_.y() + 1);
  }
}

void ScrollWindow::scrollUp() {
  if (canScrollUp()) {
    posPad_.setY(posPad_.y() - 1);
  }
}

void ScrollWindow::moveCursorDown() {
  if (!isCursorOnLastLine()) {
    resetCursorLineStyle();
    posCursor_.setY(posCursor_.y() + 1);
    if (isCursorOnLastVisibleLine()) {
      scrollDown();
    }
  }
}

void ScrollWindow::moveCursorUp() {
  if (!isCursorOnFirstLine()) {
    resetCursorLineStyle();
    posCursor_.setY(posCursor_.y() - 1);
    if (isCursorOnFirstVisibleLine()) {
      scrollUp();
    }
  }
}

void ScrollWindow::resetCursorPosition() {
  posCursor_.setX(0);
  posCursor_.setY(0);
  posPad_.setY(0);
}

void ScrollWindow::colorizeCurrentLine(short cursesColorNum) const {
  mvwchgat(pad_,
           posCursor_.y(),
           0,
           sizePad_.width(),
           A_NORMAL,
           cursesColorNum,
           nullptr);
}

void ScrollWindow::createPad() {
  pad_ = newpad(sizePad_.height(), sizePad_.width());
}

void ScrollWindow::extendPrintArea() {
  if (posPrint_.y() == sizePad_.height() - 1) {
    sizePad_.setHeight(sizePad_.height() * 2);
    wresize(pad_, sizePad_.height(), sizePad_.width());
  }
}

void ScrollWindow::clearPrintArea() {
  werase(pad_);
  posPrint_.setY(0);
}

void ScrollWindow::drawScrollBar() const {
  Style style;
  style.color = Style::Color::cyan;
  bool hasBorders = Window::style().borders;

  Point posBarUp;
  posBarUp.setX(size().width() - 1 - (hasBorders ? 1 : 0));
  posBarUp.setY(hasBorders ? 1 : 0);
  if (canScrollUp()) {
    Window::print(ACS_UARROW | A_BOLD, posBarUp, style);
  } else {
    Window::print(' ', posBarUp);
  }

  Point posBarDown;
  posBarDown.setX(size().width() - 1 -(hasBorders ? 1 : 0));
  posBarDown.setY(size().height() - 1 - (hasBorders ? 1 : 0));
  if (canScrollDown()) {
    Window::print(ACS_DARROW | A_BOLD, posBarDown, style);
  } else {
    Window::print(' ', posBarDown);
  }
}

void ScrollWindow::applyCursorLineStyle() const {
  if (style().highlight) {
    mvwchgat(pad_, posCursor_.y(), 0, sizePad_.width(), A_REVERSE, 0, nullptr);
  }
}

void ScrollWindow::resetCursorLineStyle() const {
  mvwchgat(pad_, posCursor_.y(), 0, sizePad_.width(), A_NORMAL, 0, nullptr);
}

bool ScrollWindow::isCursorOnFirstLine() const {
  return posCursor_.y() == 0;
}

bool ScrollWindow::isCursorOnLastLine() const {
  return posCursor_.y() == posPrint_.y() - (style().borders ? 1 : 0);
}

bool ScrollWindow::isCursorOnFirstVisibleLine() const {
  return posCursor_.y() == posPad_.y() - (style().borders ? 1 : 0);
}

bool ScrollWindow::isCursorOnLastVisibleLine() const {
  return posCursor_.y() == size().height() + posPad_.y() - (style().borders ? 2 : 0);
}

bool ScrollWindow::canScrollUp() const {
  return posPad_.y() > 0;
}

bool ScrollWindow::canScrollDown() const {
  return posPrint_.y() - posPad_.y() > size().height() - (style().borders ? 2 : 0);
}

}
}
