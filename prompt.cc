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

#include <tuple>

#include "event.h"

#include "prompt.h"

namespace gfx {

Prompt::Prompt(Point pos, unsigned long len)
    : origin_(pos),
      cursorPos_(pos),
      length_(len) {}

std::string Prompt::getInput() {
  for (unsigned long x = origin_.x; x < origin_.x + length_; ++x)
    Gfx::instance().setAttributes({x, origin_.y}, ' ', ATTR_UNDERLINE);
  drawCursor();

  portal::Event ev;
  std::string input;

  for (;;) {
    gfx::Gfx::instance().refresh();

    portal::Event::Type evType;
    char c;

    std::tie(evType, c) = ev.getRawInput();
    switch (evType) {
      case portal::Event::Type::select:
        return input;
      case portal::Event::Type::keyBackspace:
        input.pop_back();
        moveCursorBackward();
        break;
      default:
        input.push_back(c);
        Gfx::instance().plot(cursorPos_, c);
        moveCursorForward();
        break;
    } 
  }
}

void Prompt::drawCursor() const {
  Gfx::instance().plot(cursorPos_, ' ', ATTR_REVERSE);
}

void Prompt::moveCursorForward() {
  ++cursorPos_.x;
  drawCursor();
}

void Prompt::moveCursorBackward() {
  Gfx::instance().plot(cursorPos_, ' ', ATTR_NORMAL);
  --cursorPos_.x;
  drawCursor();
}

}
