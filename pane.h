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

#include <memory>
#include <string>

#include "point.h"
#include "size.h"
#include "style.h"

namespace portal {
namespace gfx {

class Pane {
 public:

  enum class Align {
    left,
    center,
    right
  };

  Pane(const Size& size, const Point& pos);
  ~Pane();

  Size size() const;

  // XXX define a bitset with all possible styles
  void cursorLineHighlight(bool highlight);
  void cursorLineUnderline(bool underline);
  void borders(bool borders);
  int  getCursorRowNum() const;
  void draw() const;
  void clear();
  void newline();
  void print(const std::string& line, Align align = Align::left);
  void print(int c, int cursesColorNum = 0);
  void printStatus(const std::string& status, int cursesColorNum = Style::Color::none) const;
  void clearStatus() const;
  void scrollDown();
  void scrollUp();
  void moveCursor(const Point& pos);
  void moveCursorDown();
  void moveCursorUp();
  void resetCursorPosition();
  void colorizeCurrentLine(short cursesColorNum) const;

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;

  Pane(const Pane&) = delete;
  void operator=(const Pane&) = delete;
};

}
}
