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

#include <curses.h>

namespace portal {
namespace gfx {

class Point {
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
  int  x() const {return x_;}
  int  y() const {return y_;}

private:
  int x_ {0};
  int y_ {0};
};

}
}
