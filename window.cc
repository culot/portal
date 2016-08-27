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

#include "window.h"


namespace tui {

class Window::Impl {
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
     |      +-  +-----------------+ -  -  -  -  |-v -  -  - |-  -  -  -v p
     |      |   |abc              |             | |posPad.y |          | o
     |  s . |   +-----------------+ --+ -  -  - |-^ - -v - -^          | s
     |  i h |   |def              |   |sizeView |      |               | P
     |  z e |   |ghi              |   |.height  |      |posCursor.y    | r
     |  e i |   |jkl              |   |         |      |               | i
     |  P g |   |mno##############| - | -  -  - |-  - -^               | n
     |  a h |   +-----------------+  -+         |                      | t
     |  d t |   |pqr              |             |                      | .
     |      |   |stu              |             |                      | y
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

  bool cursorLineHighlighted {true};

  void createWindow();
  void extendPrintAre();
  void highlightCursorLine();
  void unhighlightCursorLine();
};


Window::Window() : impl_{new Impl} {
  impl_->createWindow();
}

Window::Window(const Size& size, const Pos& pos) : impl_{new Impl} {
  impl_->sizeView = size;
  impl_->sizePad.width = size.width - 2;
  impl_->sizePad.height = size.width - 2;
  impl_->posView = pos;
  impl_->createWindow();
}

void Window::Impl::createWindow() {
  win = newwin(sizeView.height, sizeView.width, posView.y, posView.x);
  pad = newpad(sizePad.height, sizePad.width);
  box(win, 0, 0);
  wnoutrefresh(win);
}

Window::~Window() {
  delwin(impl_->pad);
  delwin(impl_->win);
}

void Window::cursorLineHighlighted(bool highlight) {
  impl_->cursorLineHighlighted = highlight;
}

void Window::draw() {
  impl_->highlightCursorLine();
  pnoutrefresh(impl_->pad,
               impl_->posPad.y,
               impl_->posPad.x,
               impl_->posView.y + 1,
               impl_->posView.x + 1,
               impl_->posView.y + impl_->sizeView.height - 2,
               impl_->posView.x + impl_->sizeView.width - 2);
  refresh();
}

void Window::print(const std::string& line) {
  impl_->extendPrintAre();
  mvwaddstr(impl_->pad, impl_->posPrint.y++, impl_->posPad.x, line.c_str());
}

void Window::scrollDown() {
  if (impl_->posCursor.y >= impl_->sizeView.height - 2
      && impl_->posPrint.y > impl_->posPad.y + impl_->sizeView.height) {
    ++impl_->posPad.y;
  }
}

void Window::scrollUp() {
  if (impl_->posCursor.y < impl_->posPad.y && impl_->posPad.y > 0) {
    --impl_->posPad.y;
  }
}

  void Window::moveCursorDown() {
  if (impl_->posCursor.y + impl_->posPad.y < impl_->sizePad.height
      && impl_->posPad.y + impl_->posCursor.y - 1 < impl_->posPrint.y) {
    impl_->unhighlightCursorLine();
    ++impl_->posCursor.y;
  }
  scrollDown();
}

void Window::moveCursorUp() {
  if (impl_->posCursor.y > 0) {
    impl_->unhighlightCursorLine();
    --impl_->posCursor.y;
  }
  scrollUp();
}

void Window::Impl::extendPrintAre() {
  if (posPrint.y >= sizePad.height) {
    sizePad.height *= 2;
    wresize(pad, sizePad.height, sizePad.width);
  }
}

void Window::Impl::highlightCursorLine() {
  if (cursorLineHighlighted) {
    mvwchgat(pad, posCursor.y, 0, sizePad.width, A_REVERSE, 0, nullptr);
  }
}

void Window::Impl::unhighlightCursorLine() {
  if (cursorLineHighlighted) {
    mvwchgat(pad, posCursor.y, 0, sizePad.width, A_NORMAL, 0, nullptr);
  }
}

}
