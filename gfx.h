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

namespace portal {
namespace gfx {

using Attr = uint32_t;

extern const Attr ATTR_NORMAL;
extern const Attr ATTR_BOLD;
extern const Attr ATTR_REVERSE;
extern const Attr ATTR_UNDERLINE;

using Color = uint32_t;

namespace color {
  extern const Color DEFAULT;
  extern const Color BLACK;
  extern const Color RED;
  extern const Color GREEN;
  extern const Color YELLOW;
  extern const Color BLUE;
  extern const Color MAGENTA;
  extern const Color CYAN;
  extern const Color WHITE;
}

struct Point {
  int x;
  int y;
};

struct Size {
  int height;
  int width;
};

class Gfx {
 public:
  static Gfx&  instance() {static Gfx instance_; return instance_;}
  void         refresh() const;
  void         reinit() const;
  struct Size  getScreenSize() const;
  void         plot(Point point, uint32_t symbol, Attr attr = ATTR_NORMAL) const;
  void         write(Point point, const std::string& str, Attr attr = ATTR_NORMAL) const;
  void         setAttributes(Point point, Attr fg, Attr bg) const;

 private:
  // XXX Record window size to avoid calling getScreenSize() all the time
  struct Window*      mainWin_;

  Gfx();
  ~Gfx();
  Gfx(const Gfx&) = delete;
  void operator=(const Gfx&) = delete;

  uint32_t     getCharacter(Point point) const;
};

}
}
