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

#include "termbox.h"
#include "event.h"

namespace portal {

Event::Event() {
  if (0 > tb_height())
    throw std::runtime_error("Event::Event(): termbox library not initialized");
}

std::tuple<Event::Type, char> Event::getRawInput() const {
  char c = '\0';
  struct tb_event event;
  tb_poll_event(&event);

  switch (event.type) {
    case TB_EVENT_KEY:
      switch (event.key) {
        case 0:
          c = event.ch;
          return std::make_tuple(Type::keyPressed, c);
        case TB_KEY_BACKSPACE:
        case TB_KEY_BACKSPACE2:
          return std::make_tuple(Type::keyBackspace, c);
        case TB_KEY_ENTER:
          return std::make_tuple(Type::select, c);
        default:
          return std::make_tuple(Type::unknown, c);
      }
      break;
    default:
      return std::make_tuple(Type::unknown, c);
  }
}

Event::Type Event::poll() const {
  struct tb_event event;
  tb_poll_event(&event);

  switch (event.type) {
    case TB_EVENT_KEY:
      switch (event.key) {
        case 0:
          switch (event.ch) {
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
            default:
              return Type::unknown;
          }
        case TB_KEY_BACKSPACE:
          return Type::keyBackspace;
        case TB_KEY_ARROW_DOWN:
        case TB_KEY_MOUSE_WHEEL_DOWN:
          return Type::keyDown;
        case TB_KEY_ARROW_UP:
        case TB_KEY_MOUSE_WHEEL_UP:
          return Type::keyUp;
        case TB_KEY_ENTER:
        case TB_KEY_SPACE:
          return Type::select;
        default:
          return Type::unknown;
      }
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
  }
}

}
