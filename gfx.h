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

#include <cstdint>
#include <string>

namespace gfx {

using Sprite = uint32_t;

extern Sprite SP_BOT_LEFT;
extern Sprite SP_BOT_RIGHT;
extern Sprite SP_TOP_LEFT;
extern Sprite SP_TOP_RIGHT;
extern Sprite SP_HORIZ_BAR;
extern Sprite SP_VERT_BAR;
extern Sprite SP_DBLE_HORIZ_BAR;
extern Sprite SP_DBLE_VERT_BAR;
extern Sprite SP_HORIZ_OPENG_BAR;
extern Sprite SP_HORIZ_CLOSEG_BAR;
extern Sprite SP_SCROLL_ARROW_UP;
extern Sprite SP_SCROLL_ARROW_DOWN;

using Attr = uint32_t;

extern const Attr ATTR_NORMAL;
extern const Attr ATTR_BOLD;
extern const Attr ATTR_REVERSE;
extern const Attr ATTR_UNDERLINE;

using Color = uint32_t;

extern const Color COLOR_DEFAULT;
extern const Color COLOR_BLACK;
extern const Color COLOR_RED;
extern const Color COLOR_GREEN;
extern const Color COLOR_YELLOW;
extern const Color COLOR_BLUE;
extern const Color COLOR_MAGENTA;
extern const Color COLOR_CYAN;
extern const Color COLOR_WHITE;

struct Point {
  unsigned long x;
  unsigned long y;
};

struct Size {
  unsigned long height;
  unsigned long width;
};

void useAsciiOnly();

class Gfx {
 public:
  static Gfx&  instance() {static Gfx instance_; return instance_;}
  void         refresh() const;
  void         reinit() const;
  struct Size  getScreenSize() const;
  void         plot(Point point,
                    uint32_t symbol,
                    Attr fg = ATTR_NORMAL,
                    Attr bg = ATTR_NORMAL) const;
  void         write(Point point, 
                     const std::string& str, 
                     Attr fg = ATTR_NORMAL,
                     Attr bg = ATTR_NORMAL) const;
  void         setAttributes(Point point, Attr fg, Attr bg) const;

 private:
  Gfx();
  ~Gfx();
  Gfx(const Gfx&) = delete;
  void operator=(const Gfx&) = delete;

  uint32_t     getCharacter(Point point) const;
};

}
