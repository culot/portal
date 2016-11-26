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

#include "gfx.h"
#include "window.h"

namespace portal {
namespace gfx {

class ScrollWindow : public Window {
 public:

  ScrollWindow(const Size& size, const Point& pos);
  ~ScrollWindow();

  // XXX define a bitset with all possible styles
  void cursorLineHighlight(bool highlight);
  void borders(bool borders);
  int  getCursorRowNum() const;
  void draw() const;
  void clear();
  void newline();
  void print(const std::string& line, const Style& style = {});
  void scrollDown();
  void scrollUp();
  void moveCursor(const Point& pos);
  void moveCursorDown();
  void moveCursorUp();
  void resetCursorPosition();
  void colorizeCurrentLine(short cursesColorNum) const;

 private:
  // Need to use both an curses win structure (to be able to draw the border),
  // and a curses pad structure (to display the window contents).
  // If only using a pad, and applying the border directly to it, the bottom
  // line of the border gets overwritten with the pad content.
  WINDOW* pad_ {nullptr};

  /*
     +------------------------------------------+-  -  -  - v posView.y
     |                                          |           |
     |      +-  +-----------------+ -  -  -  -  |-v -  -  - |- v-  -  -v p
     |      |   |abc              |             | |posPad.y |  | p     | o
     |  s . |   +-----------------+ --+ -  -  - |-^ - - - - ^  | oC    | s
     |  i h |   |def              |   |sizeView |              | su    | P
     |  z e |   |ghi              |   |.height  |              |  r    | r
     |  e i |   |jkl              |   |         |              |  s    | i
     |  P g |   |mno##############| - | -  -  - |-  - - - - - -^  o    | n
     |  a h |   +-----------------+  -+         |                 r    | t
     |  d t |   |pqr              |             |                 .    | .
     |      |   |stu              |             |                 y    | y
     |      |   |                 | -  -  -  -  |-  -  -  -  -  -  -  -^
     |      +-  +-----------------+             |
     |                                          |
     |                                   screen |
     +------------------------------------------+

     Note:
       posView and sizeView are inherited from Window class
       (use position() and size() to obtain them, respectively)
  */

  Size  sizePad_;
  Point posPad_;
  Point posCursor_;
  Point posPrint_;

  bool cursorLineHighlight_ {true};
  bool borders_             {false};

  void createPad();
  void extendPrintArea();
  void clearPrintArea();
  void drawScrollBar() const;
  void applyCursorLineStyle() const;
  void resetCursorLineStyle() const;
  bool isCursorOnFirstLine() const;
  bool isCursorOnLastLine() const;
  bool isCursorOnFirstVisibleLine() const;
  bool isCursorOnLastVisibleLine() const;
  bool canScrollUp() const;
  bool canScrollDown() const;
};

}
}
