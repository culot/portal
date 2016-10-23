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
#include <curses.h>

#include "event.h"
#include "size.h"
#include "inputwindow.h"

namespace portal {
namespace gfx {

InputWindow::InputWindow(const Point& pos, int len)
  : Window() {
  Size size;
  size.setHeight(3);
  size.setWidth(len + 2);
  setSize(size);

  Point position;
  position.setX(pos.x() - 1);
  position.setY(pos.y() - 1);
  setPosition(position);

  Style style;
  style.highlight = false;
  style.underline = true;
  setStyle(style);

  showBorders(false);

  draw();
}

std::string InputWindow::getInput() {
  for (;;) {
    portal::Event event;
    event.poll();
    clear();
    switch (event.type()) {
      case portal::Event::Type::enter:
        return content_;
      case portal::Event::Type::keyBackspace:
        content_.pop_back();
        break;
      case portal::Event::Type::character:
        content_.push_back(event.character());
        break;
      default:
        // DO NOTHING
        break;
    }
    print(content_);
    draw();
    doupdate();
  }
}

void InputWindow::setContent(const std::string& content) {
  content_ = content;
  print(content_);
  draw();
}

}
}