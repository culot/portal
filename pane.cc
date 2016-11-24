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

#include "pane.h"


namespace portal {
namespace gfx {

Pane::Pane(const Size& size, const Point& pos) : Window(size, pos) {
  sizePad_.setWidth(size.width());
  sizePad_.setHeight(size.height());
  createPad();
  Window::draw();
}

Pane::~Pane() {
  delwin(pad_);
}

void Pane::cursorLineHighlight(bool highlight) {
  cursorLineHighlight_ = highlight;
}

void Pane::cursorLineUnderline(bool underline) {
  cursorLineUnderline_ = underline;
}

void Pane::borders(bool borders) {
  borders_ = borders;
  Style windowStyle = style();
  if (windowStyle.borders != borders) {
    Window::clear();
    windowStyle.borders = borders;
    setStyle(windowStyle);
  }
}

int Pane::getCursorRowNum() const {
  return posCursor_.y();
}

void Pane::draw() const {
  Window::draw();
  applyCursorLineStyle();
  drawScrollBar();
  pnoutrefresh(pad_,
               posPad_.y(),
               posPad_.x(),
               position().y() + (borders_ ? 1 : 0),
               position().x() + 1,
               position().y() + size().height() - (borders_ ? 2 : 0),
               position().x() + size().width() - 2);
}

void Pane::clear() {
  clearPrintArea();
}

void Pane::newline() {
  extendPrintArea();
  posPrint_.setY(posPrint_.y() + 1);
}

void Pane::print(const std::string& line, const Style& style) {
  int xpos;
  switch (style.align) {
  case Style::Alignment::left:
    xpos = posPad_.x();
    break;
  case Style::Alignment::center:
    xpos = (size().width() - line.length()) / 2;
    break;
  case Style::Alignment::right:
    xpos = size().width() - line.length() - 2 - (borders_ ? 2 : 0);
    break;
  }

  mvwaddstr(pad_, posPrint_.y(), xpos, line.c_str());
  draw();
}

void Pane::print(int c, const Style& style) {
  waddch(pad_, c | COLOR_PAIR(style.color));
  draw();
}

void Pane::scrollDown() {
  if (canScrollDown()) {
    posPad_.setY(posPad_.y() + 1);
  }
}

void Pane::scrollUp() {
  if (canScrollUp()) {
    posPad_.setY(posPad_.y() - 1);
  }
}

void Pane::moveCursor(const Point& pos) {
  posCursor_ = pos;
}

void Pane::moveCursorDown() {
  if (!isCursorOnLastLine()) {
    resetCursorLineStyle();
    posCursor_.setY(posCursor_.y() + 1);
    if (isCursorOnLastVisibleLine()) {
      scrollDown();
    }
  }
}

void Pane::moveCursorUp() {
  if (!isCursorOnFirstLine()) {
    resetCursorLineStyle();
    posCursor_.setY(posCursor_.y() - 1);
    if (isCursorOnFirstVisibleLine()) {
      scrollUp();
    }
  }
}

void Pane::resetCursorPosition() {
  posCursor_.setX(0);
  posCursor_.setY(0);
  posPad_.setY(0);
}

void Pane::colorizeCurrentLine(short cursesColorNum) const {
  mvwchgat(pad_,
           posCursor_.y(),
           0,
           sizePad_.width(),
           A_NORMAL,
           cursesColorNum,
           nullptr);
}

void Pane::createPad() {
  pad_ = newpad(sizePad_.height(), sizePad_.width());
}

void Pane::extendPrintArea() {
  if (posPrint_.y() == sizePad_.height() - 1) {
    sizePad_.setHeight(sizePad_.height() * 2);
    wresize(pad_, sizePad_.height(), sizePad_.width());
  }
}

void Pane::clearPrintArea() {
  werase(pad_);
  posPrint_.setY(0);
}

void Pane::drawScrollBar() const {
  /* XXX Handle scrollbar
  if (canScrollUp()) {
    mvwaddch(win,
             (borders ? 1 : 0),
             sizeView.width() - 1 - (borders ? 1 : 0),
             ACS_UARROW | A_BOLD);
  }
  if (canScrollDown()) {
    mvwaddch(win,
             sizeView.height() - 1 - (borders ? 1 : 0),
             sizeView.width() - 1 - (borders ? 1 : 0),
             ACS_DARROW | A_BOLD);
  }
  */
}

void Pane::applyCursorLineStyle() const {
  if (cursorLineHighlight_) {
    mvwchgat(pad_, posCursor_.y(), 0, sizePad_.width(), A_REVERSE, 0, nullptr);
  }
  if (cursorLineUnderline_) {
    mvwchgat(pad_, posCursor_.y(), 0, sizePad_.width(), A_UNDERLINE, 0, nullptr);
  }
}

void Pane::resetCursorLineStyle() const {
  mvwchgat(pad_, posCursor_.y(), 0, sizePad_.width(), A_NORMAL, 0, nullptr);
}

bool Pane::isCursorOnFirstLine() const {
  return posCursor_.y() == 0;
}

bool Pane::isCursorOnLastLine() const {
return posCursor_.y() == posPrint_.y() - (borders_ ? 1 : 0);
}

bool Pane::isCursorOnFirstVisibleLine() const {
  return posCursor_.y() == posPad_.y() - (borders_ ? 1 : 0);
}

bool Pane::isCursorOnLastVisibleLine() const {
  return posCursor_.y() == size().height() + posPad_.y() - (borders_ ? 2 : 0);
}

bool Pane::canScrollUp() const {
  return posPad_.y() > 0;
}

bool Pane::canScrollDown() const {
  return posPrint_.y() - posPad_.y() > size().height() - (borders_ ? 2 : 0);
}

}
}
