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


namespace portal {
namespace gfx {

class Window::Impl {
 public:
  WINDOW* win {nullptr};

  Size  size;
  Point pos;
  bool  borders {true};

  void create();
  void destroy();
  void toggleBorders();
  void draw();
  void drawBorders();
  void print(const std::string& msg);
};


Window::Window(const Size& size, const Point& pos) : impl_{new Impl} {
  impl_->size = size;
  impl_->pos = pos;
  impl_->create();
  impl_->draw();
}

Window::~Window() {
  impl_->destroy();
}

Size Window::size() const {
  return impl_->size;
}

Point Window::position() const {
  return impl_->pos;
}

void Window::showBorders(bool borders) {
  if (impl_->borders != borders) {
    impl_->toggleBorders();
  }
}

void Window::print(const std::string& msg) {
  impl_->print(msg);
  draw();
}

void Window::draw() {
  impl_->draw();
}

void Window::Impl::create() {
  win = newwin(size.height(), size.width(), pos.y(), pos.x());
}

void Window::Impl::destroy() {
  delwin(win);
}

void Window::Impl::toggleBorders() {
  borders = !borders;
  draw();
}

void Window::Impl::draw() {
  drawBorders();
  wnoutrefresh(win);
}

void Window::Impl::drawBorders() {
  if (borders) {
    box(win, 0, 0);
  }
}

void Window::Impl::print(const std::string& msg) {
  int x = pos.x() + (borders ? 1 : 0);
  mvwaddstr(win, pos.y(), x, msg.c_str());
}

}
}
