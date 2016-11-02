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

#pragma once

#include <thread>
#include <curses.h>

namespace portal {
namespace gfx {

struct Point {
public:
  enum class Label {
    topLeft,
    topRight,
    bottomLeft,
    bottomRight,
    center
  };

  Point() {}
  Point(Label label) {
    switch (label) {
    case Label::topLeft:
      x_ = 0;
      y_ = 0;
      break;
    case Label::topRight:
      x_ = COLS;
      y_ = 0;
      break;
    case Label::bottomLeft:
      x_ = 0;
      y_ = LINES;
      break;
    case Label::bottomRight:
      x_ = COLS;
      y_ = LINES;
      break;
    case Label::center:
      x_ = COLS / 2;
      y_ = LINES / 2;
      break;
    }
  }

  void setX(int x) {x_ = x;}
  void setY(int y) {y_ = y;}
  void reset() {x_ = 0; y_ = 0;}
  bool isNull() const {return x_ == 0 && y_ == 0;}
  int  x() const {return x_;}
  int  y() const {return y_;}

  bool operator==(const Point& other) const {
    return y() == other.y() && x() == other.x();
  }
  bool operator!=(const Point& other) const {return !(*this == other);}

private:
  int x_ {0};
  int y_ {0};
};


struct Size {
public:
  void setWidth(int width) {width_ = width;}
  void setHeight(int height) {height_ = height;}
  void reset() {height_ = 0; width_ = 0;}
  int  width() const {return width_;}
  int  height() const {return height_;}
  bool isNull() const {return width_ == 0 && height_ == 0;}

  bool operator==(const Size& other) const {
    return height() == other.height() && width() == other.width();
  }
  bool operator!=(const Size& other) const {return !(*this == other);}

private:
  int width_  {0};
  int height_ {0};
};


struct Style {

   enum Color {
     none,
     black,
     cyan,
     magenta,
     red,
     yellow,
     blue,
     cyanOnBlue
  };

  bool   bold      {false};
  bool   underline {false};
  bool   highlight {false};
  bool   borders   {false};
  Color  color     {Color::none};
};


class Gfx {
 public:
  static Gfx&  instance() {static Gfx instance_; return instance_;}

  void         init();
  void         update();
  void         terminate();

 private:
  std::mutex  mutex_;

  Gfx() {}
  ~Gfx() {}
  Gfx(const Gfx&) = delete;
  void operator=(const Gfx&) = delete;
};

}
}
