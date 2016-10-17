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

#include "size.h"
#include "tray.h"

namespace portal {
namespace gfx {

Tray::Tray(const Point& center, int nbSlots)
  : nbSlots_(nbSlots) {
  Size paneSize;
  paneSize.setHeight(1);
  paneSize.setWidth(nbSlots * 2 + 1);
  Point panePos;
  panePos.setX(center.x() - paneSize.width() / 2);
  panePos.setY(center.y() - 1);

  pane_ = std::unique_ptr<Pane>(new Pane(paneSize, panePos));
  pane_->borders(false);
  pane_->cursorLineHighlight(false);
  pane_->cursorLineUnderline(false);
  draw();
}

void Tray::draw() const {
  pane_->clear();
  drawSlots();
  pane_->draw();
}

void Tray::drawSlots() const {
  for (int i = 0; i < nbSlots_; ++i) {
    pane_->printChar(ACS_DIAMOND, (i == selectedSlotNum_ ? 2 : 0));
    pane_->printChar(' ');
  }
}

void Tray::selectSlot(int slotNum) {
  if (slotNum >= nbSlots_ || slotNum < 0) {
    throw std::out_of_range("Tray::selectSlot(): invalid slotNum ["
                            + std::to_string(slotNum) + "]");
  }
  selectedSlotNum_ = slotNum;
  draw();
}

void Tray::selectNextSlot() {
  ++selectedSlotNum_;
  if (selectedSlotNum_ >= nbSlots_) {
    selectedSlotNum_ = 0;
  }
  draw();
}

void Tray::selectPreviousSlot() {
  --selectedSlotNum_;
  if (selectedSlotNum_ < 0) {
    selectedSlotNum_ = nbSlots_ - 1;
  }
  draw();
}

}
}
