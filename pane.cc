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

class Pane::Impl {
 public:
  // Need to use both an curses win structure (to be able to draw the border),
  // and a curses pad structure (to display the window contents.
  // If only using a pad, and applying the border directly to it, the bottom
  // line of the border gets overwritten with the pad content.
  WINDOW* win {nullptr};
  WINDOW* pad {nullptr};

  /*
     +------------------------------------------+-  -  -  - v posView.y
     |                                          |           |
     |      +-  +-----------------+ -  -  -  -  |-v -  -  - |- v-  -  -v p
     |      |   |abc              |             | |posPad.y |  | p     | o
     |  s . |   +-----------------+ --+ -  -  - |-^ - - - - ^  | oC    | s
     |  i h |   |def              |   |sizeView |              | su    | P
     |  z e |   |ghi              |   |.height  |              |  r    | r
     |  e i |   |jkl              |   |         |              |  s    | i
     |  P g |   |mno##############| - | -  -  - |-  - - - - - -^  o    | n
     |  a h |   +-----------------+  -+         |                 r    | t
     |  d t |   |pqr              |             |                 .    | .
     |      |   |stu              |             |                 y    | y
     |      |   |                 | -  -  -  -  |-  -  -  -  -  -  -  -^
     |      +-  +-----------------+             |
     |                                          |
     |                                   screen |
     +------------------------------------------+
  */
  Size sizePad;
  Size sizeView;

  Point posPad;
  Point posView;
  Point posCursor;
  Point posPrint;

  bool cursorLineHighlight {true};
  bool borders             {true};

  void createPane();
  void extendPrintArea();
  void clearPrintArea();
  void drawScrollBar();
  void highlightCursorLine();
  void unhighlightCursorLine();
  void toggleBorders();
  bool isCursorOnFirstLine() const;
  bool isCursorOnLastLine() const;
  bool isCursorOnFirstVisibleLine() const;
  bool isCursorOnLastVisibleLine() const;
  bool canScrollUp() const;
  bool canScrollDown() const;
};


/*
Pane::Pane() : impl_{new Impl} {
  impl_->createPane();
}
*/

Pane::Pane(const Size& size, const Point& pos) : impl_{new Impl} {
  impl_->sizeView = size;
  impl_->sizePad.setWidth(size.width() - 2);
  impl_->sizePad.setHeight(size.width() - 2);
  impl_->posView = pos;
  impl_->createPane();
}

void Pane::Impl::createPane() {
  win = newwin(sizeView.height(), sizeView.width(), posView.y(), posView.x());
  pad = newpad(sizePad.height(), sizePad.width());
  box(win, 0, 0);
  wnoutrefresh(win);
}

Pane::~Pane() {
  delwin(impl_->pad);
  delwin(impl_->win);
}

void Pane::cursorLineHighlight(bool highlight) {
  impl_->cursorLineHighlight = highlight;
}

void Pane::borders(bool borders) {
  if (impl_->borders != borders) {
    werase(impl_->win);
    impl_->toggleBorders();
    draw();
  }
}

int Pane::getCursorRowNum() const {
  return impl_->posCursor.y();
}

void Pane::draw() {
  impl_->highlightCursorLine();
  impl_->drawScrollBar();
  pnoutrefresh(impl_->pad,
               impl_->posPad.y(),
               impl_->posPad.x(),
               impl_->posView.y() + (impl_->borders ? 1 : 0),
               impl_->posView.x() + 1,
               impl_->posView.y() + impl_->sizeView.height() - (impl_->borders ? 2 : 0),
               impl_->posView.x() + impl_->sizeView.width() - 2);
  wnoutrefresh(impl_->win);
}

void Pane::clear() {
  impl_->clearPrintArea();
}

void Pane::newline() {
  impl_->extendPrintArea();
  impl_->posPrint.setY(impl_->posPrint.y() + 1);
}

void Pane::print(const std::string& line, Align align) {
  int xpos;
  switch (align) {
  case Align::left:
    xpos = impl_->posPad.x();
    break;
  case Align::center:
    xpos = (impl_->sizeView.width() - line.length()) / 2;
    break;
  case Align::right:
    xpos = impl_->sizeView.width() - line.length() - 2 - (impl_->borders ? 1 : 0);
    break;
  }

  mvwaddstr(impl_->pad, impl_->posPrint.y(), xpos, line.c_str());
}

void Pane::scrollDown() {
  if (impl_->isCursorOnLastVisibleLine()) {
    impl_->posPad.setY(impl_->posPad.y() + 1);
  }
}

void Pane::scrollUp() {
  if (impl_->isCursorOnFirstVisibleLine()) {
    impl_->posPad.setY(impl_->posPad.y() - 1);
  }
}

void Pane::moveCursorDown() {
  if (!impl_->isCursorOnLastLine()) {
    impl_->unhighlightCursorLine();
    impl_->posCursor.setY(impl_->posCursor.y() + 1);
    scrollDown();    
  }
}

void Pane::moveCursorUp() {
  if (!impl_->isCursorOnFirstLine()) {
    impl_->unhighlightCursorLine();
    impl_->posCursor.setY(impl_->posCursor.y() - 1);
    scrollUp();    
  }
}

void Pane::resetCursorPosition() {
  impl_->posCursor.setY(0);
  impl_->posPad.setY(0);
}

void Pane::colorizeCurrentLine(short cursesColorNum) const {
  mvwchgat(impl_->pad,
           impl_->posCursor.y(),
           0,
           impl_->sizePad.width(),
           A_NORMAL,
           cursesColorNum,
           nullptr);
}

void Pane::Impl::clearPrintArea() {
  werase(pad);
  posPrint.setY(0);
}

void Pane::Impl::drawScrollBar() {
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
}

void Pane::Impl::extendPrintArea() {
  if (posPrint.y() >= sizePad.height()) {
    sizePad.setHeight(sizePad.height() * 2);
    wresize(pad, sizePad.height(), sizePad.width());
  }
}

void Pane::Impl::highlightCursorLine() {
  if (cursorLineHighlight) {
    mvwchgat(pad, posCursor.y(), 0, sizePad.width(), A_REVERSE, 0, nullptr);
  }
}

void Pane::Impl::unhighlightCursorLine() {
  if (cursorLineHighlight) {
    mvwchgat(pad, posCursor.y(), 0, sizePad.width(), A_NORMAL, 0, nullptr);
  }
}

void Pane::Impl::toggleBorders() {
  /*
  if (borders) {
    // You're right, borders are overrated, let's get rid of those
    sizePad.width += 2;
    sizePad.height += 2;
    borders = false;
  } else {
    // Gimme back my borders, you punk!
    sizePad.width -= 2;
    sizePad.height -= 2;
    borders = true;
  }
  */
  borders = !borders;
}

bool Pane::Impl::isCursorOnFirstLine() const {
  return posCursor.y() == 0;
}

bool Pane::Impl::isCursorOnLastLine() const {
  return posCursor.y() == posPrint.y() - (borders ? 2 : 0) - 1;
}

bool Pane::Impl::isCursorOnFirstVisibleLine() const {
  return posCursor.y() == posPad.y() - (borders ? 1 : 0);
}

bool Pane::Impl::isCursorOnLastVisibleLine() const {
  return posCursor.y() == sizeView.height() + posPad.y() - (borders ? 2 : 0);
}

bool Pane::Impl::canScrollUp() const {
  return posPad.y() > 0;
}

bool Pane::Impl::canScrollDown() const {
  return posPrint.y() - posPad.y() > sizeView.height();
}

}
}
