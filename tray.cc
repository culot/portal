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

#include "tray.h"

namespace portal {
namespace gfx {

Tray::Tray(const Point& center, int nbSlots)
  : Window(), nbSlots_(nbSlots) {
  Size size;
  size.setHeight(1);
  size.setWidth(nbSlots * 2 + 1);
  setSize(size);

  Point pos;
  pos.setX(center.x() - size.width() / 2);
  pos.setY(center.y() - 1);
  setPosition(pos);

  drawSlots();
}

void Tray::display() {
  drawSlots();
  draw();
}

void Tray::drawSlots() {
  Style normal, highlight;
  highlight.bold = true;
  highlight.color = Style::Color::magenta;
  clear();
  print(' ');
  for (auto i = 0; i < nbSlots_; ++i) {
    setStyle(i == selectedSlotNum_ ? highlight : normal);
    print(ACS_DIAMOND);
    print(' ');
  }
}

void Tray::selectSlot(int slotNum) {
  if (slotNum >= nbSlots_ || slotNum < 0) {
    throw std::out_of_range("Tray::selectSlot(): invalid slotNum ["
                            + std::to_string(slotNum) + "]");
  }
  selectedSlotNum_ = slotNum;
  display();
}

void Tray::selectNextSlot() {
  ++selectedSlotNum_;
  if (selectedSlotNum_ >= nbSlots_) {
    selectedSlotNum_ = 0;
  }
  display();
}

void Tray::selectPreviousSlot() {
  --selectedSlotNum_;
  if (selectedSlotNum_ < 0) {
    selectedSlotNum_ = nbSlots_ - 1;
  }
  display();
}

}
}
