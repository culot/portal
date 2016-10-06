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

#include <cassert>

#include <curses.h>

#include "gfx.h"

namespace portal {
namespace gfx {

const Attr ATTR_NORMAL            = A_NORMAL;
const Attr ATTR_BOLD              = A_BOLD;
const Attr ATTR_REVERSE           = A_REVERSE;
const Attr ATTR_UNDERLINE         = A_UNDERLINE;

namespace color {
  const Color DEFAULT  = COLOR_BLACK;
  const Color BLACK    = COLOR_BLACK;
  const Color RED      = COLOR_RED;
  const Color GREEN    = COLOR_GREEN;
  const Color YELLOW   = COLOR_YELLOW;
  const Color BLUE     = COLOR_BLUE;
  const Color MAGENTA  = COLOR_MAGENTA;
  const Color CYAN     = COLOR_CYAN;
  const Color WHITE    = COLOR_WHITE;
}

struct Window {
  WINDOW* ptr;
};

Gfx::Gfx() {
  initscr();
  cbreak();
  noecho();
  curs_set(0);

  struct Size screenSize = getScreenSize();
  mainWin_ = new Window();
  mainWin_->ptr = newwin(screenSize.height, screenSize.width, 0, 0);
}

Gfx::~Gfx() {
  clear();
  refresh();
  delwin(mainWin_->ptr);
  endwin();
  delete mainWin_;
}

void Gfx::reinit() const {
  // XXX still needed?
  Gfx::refresh();
}

void Gfx::refresh() const {
  wrefresh(mainWin_->ptr);
}

struct Size Gfx::getScreenSize() const {
  int height, width;
  getmaxyx(stdscr, height, width);

  return {height, width};
}

void Gfx::plot(Point point, uint32_t symbol, Attr attr) const {
  mvwaddch(mainWin_->ptr, point.y, point.x, symbol | attr);
}

void Gfx::write(Point point, const std::string& str, Attr attr) const {
  int x = point.x;

  for (const char& c : str) {
    plot({x++, point.y}, c, attr);
  }
}

// XXX To be implemented
void Gfx::setAttributes(Point point, Attr fg, Attr bg) const {
//  tb_change_cell(point.x, point.y, getCharacter(point), fg, bg);
}

uint32_t Gfx::getCharacter(Point point) const {
  return mvwinch(mainWin_->ptr, point.y, point.x) | A_CHARTEXT;
}

}
}
