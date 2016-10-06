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
  Size  sizePad    {LINES, COLS};
  Size  sizeView   {LINES, COLS};

  Pos   posPad     {0, 0};
  Pos   posView    {0, 0};
  Pos   posCursor  {0, 0};
  Pos   posPrint   {0, 0};

  bool cursorLineHighlight {true};
  bool borders             {true};

  void createPane();
  void extendPrintArea();
  void clearPrintArea();
  void highlightCursorLine();
  void unhighlightCursorLine();
  void toggleBorders();
  bool isCursorOnFirstLine() const;
  bool isCursorOnLastLine() const;
  bool isCursorOnFirstVisibleLine() const;
  bool isCursorOnLastVisibleLine() const;
};


Pane::Pane() : impl_{new Impl} {
  impl_->createPane();
}

Pane::Pane(const Size& size, const Pos& pos) : impl_{new Impl} {
  impl_->sizeView = size;
  impl_->sizePad.width = size.width - 2;
  impl_->sizePad.height = size.width - 2;
  impl_->posView = pos;
  impl_->createPane();
}

void Pane::Impl::createPane() {
  win = newwin(sizeView.height, sizeView.width, posView.y, posView.x);
  pad = newpad(sizePad.height, sizePad.width);
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
  return impl_->posCursor.y;
}

void Pane::draw() {
  impl_->highlightCursorLine();
  pnoutrefresh(impl_->pad,
               impl_->posPad.y,
               impl_->posPad.x,
               impl_->posView.y + (impl_->borders ? 1 : 0),
               impl_->posView.x + 1,
               impl_->posView.y + impl_->sizeView.height - (impl_->borders ? 2 : 0),
               impl_->posView.x + impl_->sizeView.width - 2);
  wnoutrefresh(impl_->win);
}

void Pane::clear() {
  impl_->clearPrintArea();
}

void Pane::print(const std::string& line) {
  impl_->extendPrintArea();
  mvwaddstr(impl_->pad, impl_->posPrint.y++, impl_->posPad.x, line.c_str());
}

void Pane::scrollDown() {
  if (impl_->isCursorOnLastVisibleLine()) {
    ++impl_->posPad.y;
  }
}

void Pane::scrollUp() {
  if (impl_->isCursorOnFirstVisibleLine()) {
    --impl_->posPad.y;
  }
}

void Pane::moveCursorDown() {
  if (!impl_->isCursorOnLastLine()) {
    impl_->unhighlightCursorLine();
    ++impl_->posCursor.y;
    scrollDown();    
  }
}

void Pane::moveCursorUp() {
  if (!impl_->isCursorOnFirstLine()) {
    impl_->unhighlightCursorLine();
    --impl_->posCursor.y;
    scrollUp();    
  }
}

void Pane::resetCursorPosition() {
  impl_->posCursor.y = 0;
  impl_->posPad.y = 0;
}

void Pane::colorizeCurrentLine(short cursesColorNum) const {
  mvwchgat(impl_->pad,
           impl_->posCursor.y,
           0,
           impl_->sizePad.width,
           A_NORMAL,
           cursesColorNum,
           nullptr);
}

void Pane::Impl::clearPrintArea() {
  werase(pad);
  posPrint.y = 0;
}

void Pane::Impl::extendPrintArea() {
  if (posPrint.y >= sizePad.height) {
    sizePad.height *= 2;
    wresize(pad, sizePad.height, sizePad.width);
  }
}

void Pane::Impl::highlightCursorLine() {
  if (cursorLineHighlight) {
    mvwchgat(pad, posCursor.y, 0, sizePad.width, A_REVERSE, 0, nullptr);
    wnoutrefresh(win);
  }
}

void Pane::Impl::unhighlightCursorLine() {
  if (cursorLineHighlight) {
    mvwchgat(pad, posCursor.y, 0, sizePad.width, A_NORMAL, 0, nullptr);
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
  return posCursor.y == 0;
}

bool Pane::Impl::isCursorOnLastLine() const {
  return posCursor.y == posPrint.y - (borders ? 2 : 0) - 1;
}

bool Pane::Impl::isCursorOnFirstVisibleLine() const {
  return posCursor.y == posPad.y - (borders ? 1 : 0);
}

bool Pane::Impl::isCursorOnLastVisibleLine() const {
  return posCursor.y == sizeView.height + posPad.y - (borders ? 2 : 0);
}

}
}
