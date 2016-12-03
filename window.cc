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
#include <curses.h>

#include "window.h"


namespace portal {
namespace gfx {

class Window::Impl {
 public:
  WINDOW* win {nullptr};

  Size  size;
  Point pos, posStatus;
  Style style;

  // XXX Add a Point posCursor to allow for text spanning multi lines

  bool initialized() const;
  void create();
  void resize();
  void move();
  void destroy();
  void draw() const;
  void applyStyle() const;
  void drawBorders() const;
  void print(const std::string& msg);
};


Window::Window() : impl_{new Impl} {
  impl_->create();
}

Window::Window(const Size& size, const Point& pos, const Style& style)
  : impl_{new Impl} {
  impl_->size = size;
  impl_->pos = pos;
  impl_->style = style;
  impl_->create();
  impl_->draw();
}

Window::~Window() {
  impl_->destroy();
}

void Window::setSize(const Size& size) {
  if (impl_->size != size) {
    impl_->size = size;
    impl_->resize();
  }
}

void Window::setPosition(const Point& pos) {
  if (impl_->pos != pos) {
    impl_->pos = pos;
    impl_->move();
  }
}

void Window::setStyle(const Style& style) {
  impl_->style = style;
  impl_->drawBorders();
}

Size Window::size() const {
  return impl_->size;
}

Point Window::position() const {
  return impl_->pos;
}

Style Window::style() const {
  return impl_->style;
}

  // XXX use provided style instead of class one
void Window::print(const std::string& msg, const Style& style) {
  if (impl_->style.color != Style::Color::none) {
    wattron(impl_->win, COLOR_PAIR(impl_->style.color));
  }
  impl_->print(msg);
  if (impl_->style.color != Style::Color::none) {
    wattroff(impl_->win, COLOR_PAIR(impl_->style.color));
  }
  wnoutrefresh(impl_->win);
}

void Window::print(int c, const Point& pos, const Style& style) const {
  int color =
    style.color != Style::Color::none ? style.color : impl_->style.color;
  wattron(impl_->win, COLOR_PAIR(color));
  if (!pos.isNull()) {
    wmove(impl_->win, pos.y(), pos.x());
  }
  waddch(impl_->win, c);
  wattroff(impl_->win, COLOR_PAIR(color));
  wnoutrefresh(impl_->win);
}

void Window::printStatus(const std::string& status, const Style& style) const {
  int statusLength = status.length();
  int xpos = impl_->size.width() - statusLength - 8;
  int ypos = impl_->size.height() - 1;
  impl_->posStatus.setX(xpos);
  impl_->posStatus.setY(ypos);
  mvwaddch(impl_->win, ypos, xpos++, ACS_RTEE);
  mvwaddch(impl_->win, ypos, xpos++, ' ');
  wattron(impl_->win, COLOR_PAIR(style.color) | style.cursesAttrs());
  mvwaddstr(impl_->win, ypos, xpos, status.c_str());
  wattroff(impl_->win, COLOR_PAIR(style.color) | style.cursesAttrs());
  xpos += statusLength;
  mvwaddch(impl_->win, ypos, xpos++, ' ');
  mvwaddch(impl_->win, ypos, xpos, ACS_LTEE);
  wnoutrefresh(impl_->win);
}

void Window::setStatusStyle(int xpos, int len, const Style& style) const {
  mvwchgat(impl_->win,
           impl_->posStatus.y(),
           impl_->posStatus.x() + xpos + 2,
           len,
           style.cursesAttrs(),
           style.color,
           nullptr);
  wnoutrefresh(impl_->win);
}

void Window::clearStatus() const {
  impl_->posStatus.reset();
  impl_->drawBorders();
}

void Window::draw() const {
  impl_->draw();
}

void Window::clear() {
  werase(impl_->win);
  wmove(impl_->win, 0, 0);
  impl_->draw();
}

bool Window::Impl::initialized() const {
  return win != nullptr;
}

void Window::Impl::create() {
  if (initialized()) {
    throw std::runtime_error("Window::Impl::create() - Object already initialized");
  }
  win = newwin(size.height(), size.width(), pos.y(), pos.x());
  drawBorders();
}

void Window::Impl::resize() {
  wresize(win, size.height(), size.width());
}

void Window::Impl::move() {
  mvwin(win, pos.y(), pos.x());
}

void Window::Impl::destroy() {
  if (!initialized()) {
    throw std::runtime_error("Window::Impl::destroy() - Object already destroyed");
  }
  delwin(win);
}

void Window::Impl::draw() const {
  applyStyle();
  wnoutrefresh(win);
}

void Window::Impl::applyStyle() const {
  if (style.underline) {
    mvwchgat(win, 0, 0, size.width(), A_UNDERLINE, 0, nullptr);
  }
}

void Window::Impl::drawBorders() const {
  // If a status is displayed (ie its position is not null), then we
  // must not override it by drawing the border, hence the following
  // test on posStatus.
  if (style.borders && posStatus.isNull()) {
    box(win, 0, 0);
  }
}

void Window::Impl::print(const std::string& msg) {
  int offset = style.borders ? 1 : 0;
  mvwaddstr(win, offset, offset, msg.c_str());
}

}
}
