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

#include "event.h"

namespace portal {

static constexpr int ctrl(int c) {return 0x1F & c;}

bool Event::poll() {
  character_ = getch();
  switch (character_) {
    case '\t':
      type_ = Type::nextMode;
      break;
    case ctrl('X'):
      type_ = Type::go;
      break;
    case ctrl('N'):
      type_ = Type::keyDown;
      break;
    case ctrl('P'):
      type_ = Type::keyUp;
      break;
      // XXX handle SIGINT
    case ctrl('C'):
      type_ = Type::quit;
      break;
      /* XXX handle lower-panel scrolling
    case 'J':
      type_ = Type::keyShiftDown;
      break;
    case 'K':
      type_ = Type::keyShiftUp;
      break;
      */
    case ctrl(' '):
      type_ = Type::select;
      break;
    case ctrl('D'):
      type_ = Type::deselect;
      break;
    case KEY_BACKSPACE:
      type_ = Type::keyBackspace;
      break;
    case KEY_DOWN:
      type_ = Type::keyDown;
      break;
    case KEY_UP:
      type_ = Type::keyUp;
      break;
    case KEY_PPAGE:
      type_ = Type::pageUp;
      break;
    case KEY_NPAGE:
      type_ = Type::pageDown;
      break;
    case KEY_ENTER:
    case '\n':
      type_ = Type::enter;
      break;
    case ctrl('L'):
      type_ = Type::redraw;
      break;
    default:
      type_ = Type::character;
      break;
  }

  return type_ != Type::quit;
}

}
