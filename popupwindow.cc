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

#include <thread>
#include <chrono>
#include <curses.h>

#include "popupwindow.h"

namespace portal {
namespace gfx {

PopupWindow::PopupWindow(const std::string& msg, Type type, const Point& center)
  : Window() {
  Size size;
  size.setHeight(3);
  size.setWidth(msg.length() + 2);
  setSize(size);

  Point pos;
  pos.setX(center.x() - msg.length() / 2);
  pos.setY(center.y());
  setPosition(pos);

//  showBorders(false);
  print(msg);

  /*
  switch (type) {
  case Type::brief:
    pane_->colorizeCurrentLine(2);
    break;
  case Type::info:
    pane_->colorizeCurrentLine(7);
    break;
  case Type::warning:
    pane_->colorizeCurrentLine(4);
    break;
  case Type::error:
    pane_->colorizeCurrentLine(5);
    break;
  }
  */

  draw();
  refresh();

  int duration;
  switch (type) {
  case Type::brief:
    duration = 500;
    break;
  default:
    duration = 1400;
    break;
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(duration));
}

}
}
