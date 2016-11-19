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

#include <stdexcept>

#include "gfx.h"

namespace portal {
namespace gfx {

void Gfx::init() {
  initscr();

  if (has_colors() && start_color() == OK) {
    init_pair(Style::Color::none, 0, 0);
    init_pair(Style::Color::black, COLOR_BLACK, 0);
    init_pair(Style::Color::cyan, COLOR_CYAN, 0);
    init_pair(Style::Color::magenta, COLOR_MAGENTA, 0);
    init_pair(Style::Color::red, COLOR_RED, 0);
    init_pair(Style::Color::yellow, COLOR_YELLOW, 0);
    init_pair(Style::Color::blue, COLOR_BLUE, 0);
    init_pair(Style::Color::cyanOnBlue, COLOR_CYAN, COLOR_BLUE);
  } else {
    // XXX Need to deal with B&W terminals
    throw std::runtime_error("Sorry, B&W terminals not supported yet");
  }

  raw();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);
  refresh();  // A refresh might seem unnecessary here, but user input is
              // gathered from stdscr via a call to getch, which does an
              // implicit refresh first (don't ask me why...). Hence doing
              // this refresh explicitly avoids a black screen when portal
              // starts. The black screen does not appear afterwards as
              // stdscr is never touched by portal, so curses detects it
              // does not need any subsequent refreshes.
}

// The curses library fails to handle two concurrent threads trying to
// update the display at the same time. This creates a single point of
// entry that prevents this case from happening. Morevover, a try_lock
// is used here instead of just lock, as if a thread is already
// updating the display, we assume there is no need to update it again
// right after, so we skip an update to avoir too frequent refreshes.
void Gfx::update() {
  if (mutex_.try_lock()) {
    doupdate();
    mutex_.unlock();
  }
}

void Gfx::terminate() {
  clear();
  endwin();
}

}
}
