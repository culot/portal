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

#include "prompt.h"

#include "form.h"

namespace gfx {

void Form::draw() {
  setLayout();
  erase();
  drawBorder();
  drawLabel();
}

std::string Form::getInput() const {
  Prompt prompt({area_.getXmin() + 1, area_.getYmin() + 1}, area_.getWidth());
  std::string input = prompt.getInput();

  return input;
}

void Form::erase() const {
  for (unsigned int y = area_.getYmin(); y <= area_.getYmax(); ++y)
    for (unsigned int x = area_.getXmin(); x <= area_.getXmax(); ++x)
      Gfx::instance().plot({x, y}, ' ');
}

void Form::setLayout() {
  const unsigned long xOffset = 2;

  Size screenSize = gfx::Gfx::instance().getScreenSize();
  unsigned long width = screenSize.width - 2 * xOffset;
  unsigned long height = 3;

  unsigned long x = xOffset;
  unsigned long y = screenSize.height / 2 - (height / 2);

  area_.resetLayout({x, y}, {height, width});
}

void Form::drawLabel() const {
  unsigned int x = area_.getXmin() + 2;
  unsigned int y = area_.getYmin();

  Gfx::instance().plot({x, y}, SP_HORIZ_OPENG_BAR);
  Gfx::instance().write({x + 1, y}, label_);
  Gfx::instance().plot({x + label_.length() + 1, y}, SP_HORIZ_CLOSEG_BAR);
}

void Form::drawBorder() const {
  area_.drawBorder();
}

}
