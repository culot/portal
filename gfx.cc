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

#include "termbox.h"

#include "gfx.h"

namespace gfx {

// List of usable utf8 codes:
// http://unicode-table.com/en/blocks/
Sprite SP_BOT_LEFT          = 0x2570;
Sprite SP_BOT_RIGHT         = 0x256F;
Sprite SP_TOP_LEFT          = 0x256D;
Sprite SP_TOP_RIGHT         = 0x256E;
Sprite SP_HORIZ_BAR         = 0x2500;
Sprite SP_VERT_BAR          = 0x2502;
Sprite SP_DBLE_HORIZ_BAR    = 0x2550;
Sprite SP_DBLE_VERT_BAR     = 0x2551;
Sprite SP_HORIZ_OPENG_BAR   = 0x2524;
Sprite SP_HORIZ_CLOSEG_BAR  = 0x251C;
Sprite SP_SCROLL_ARROW_UP   = 0x1F879; // slim arrow: 0x1F845;
Sprite SP_SCROLL_ARROW_DOWN = 0x1F87B; // slim arrow: 0x1F847;
 
const Attr ATTR_NORMAL            = TB_DEFAULT;
const Attr ATTR_BOLD              = TB_BOLD;
const Attr ATTR_REVERSE           = TB_REVERSE;
const Attr ATTR_UNDERLINE         = TB_UNDERLINE;

const Color COLOR_DEFAULT         = TB_DEFAULT;
const Color COLOR_BLACK           = TB_BLACK;
const Color COLOR_RED             = TB_RED;
const Color COLOR_GREEN           = TB_GREEN;
const Color COLOR_YELLOW          = TB_YELLOW;
const Color COLOR_BLUE            = TB_BLUE;
const Color COLOR_MAGENTA         = TB_MAGENTA;
const Color COLOR_CYAN            = TB_CYAN;
const Color COLOR_WHITE           = TB_WHITE;

void useAsciiOnly() {
  SP_BOT_LEFT          = '\\';
  SP_BOT_RIGHT         = '/';
  SP_TOP_LEFT          = '/';
  SP_TOP_RIGHT         = '\\';
  SP_HORIZ_BAR         = '-';
  SP_VERT_BAR          = '|';
  SP_DBLE_HORIZ_BAR    = '=';
  SP_DBLE_VERT_BAR     = '|';
  SP_HORIZ_OPENG_BAR   = '|';
  SP_HORIZ_CLOSEG_BAR  = '|';
  SP_SCROLL_ARROW_UP   = '^';
  SP_SCROLL_ARROW_DOWN = 'v';
}


Gfx::Gfx() {
  tb_init();
}

Gfx::~Gfx() {
  tb_shutdown();
}

void Gfx::reinit() const {
  tb_shutdown();
  tb_init();
}

void Gfx::refresh() const {
  tb_present();
}

struct Size Gfx::getScreenSize() const {
  unsigned int height = tb_height();
  unsigned int width  = tb_width();

  return {height, width};
}

void Gfx::plot(Point point, uint32_t symbol, Attr fg, Attr bg) const {
  tb_change_cell(point.x, point.y, symbol, fg, bg);
}

void Gfx::write(Point point, const std::string & str, Attr fg, Attr bg) const {
  unsigned int x = point.x;

  for (const char& c : str)
    tb_change_cell(x++, point.y, c, fg, bg);
}

void Gfx::setAttributes(Point point, Attr fg, Attr bg) const {
  tb_change_cell(point.x, point.y, getCharacter(point), fg, bg);
}

uint32_t Gfx::getCharacter(Point point) const {
  struct tb_cell* charArray = tb_cell_buffer();

  return charArray[point.y * tb_width() + point.x].ch;
}

}
