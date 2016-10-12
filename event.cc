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

std::tuple<Event::Type, int> Event::getRawInput() const {
  int ch = getch();
  switch (ch) {
    case KEY_BACKSPACE:
      return std::make_tuple(Type::keyBackspace, ch);
    case KEY_ENTER:
    case '\n':
      return std::make_tuple(Type::select, ch);
    default:
      return std::make_tuple(Type::keyPressed, ch);
  }
}

Event::Type Event::poll() const {
  int event = getch();
  switch (event) {
    case '/':
      return Type::search;
    case 'f':
      return Type::filter;
    case 'g':
      return Type::go;
    case 'j':
      return Type::keyDown;
    case 'k':
      return Type::keyUp;
    case 'q':
      return Type::quit;
    case 'J':
      return Type::keyShiftDown;
    case 'K':
      return Type::keyShiftUp;
    case '+':
      return Type::flagInstall;
    case '-':
      return Type::flagRemove;
    case KEY_BACKSPACE:
      return Type::keyBackspace;
    case KEY_DOWN:
      return Type::keyDown;
    case KEY_UP:
      return Type::keyUp;
    case KEY_PPAGE:
      //        case TB_KEY_CTRL_B:
      return Type::pageUp;
    case KEY_NPAGE:
      //        case TB_KEY_CTRL_F:
      return Type::pageDown;
    case KEY_ENTER:
    case '\n':
    case ' ':
      return Type::select;
      /*
         case TB_KEY_CTRL_L:
         return Type::redraw;
         */
    default:
      return Type::unknown;
  }
  /*
  case TB_EVENT_MOUSE:
      switch (event.key)
      {
        case TB_KEY_MOUSE_WHEEL_DOWN:
          return Type::keyDown;
        default:
          return Type::unknown;
      }
      break;
    default:
      return Type::unknown;
      */
}

}
