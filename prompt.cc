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

#include <tuple>
#include <curses.h>

#include "event.h"
#include "size.h"
#include "prompt.h"

namespace portal {
namespace gfx {

Prompt::Prompt(const Point& pos, int len) {
  Size paneSize;
  paneSize.setHeight(3);
  paneSize.setWidth(len + 2);
  Point panePos;
  panePos.setX(pos.x() - 1);
  panePos.setY(pos.y() - 1);

  pane_ = std::unique_ptr<Pane>(new Pane(paneSize, panePos));
  pane_->borders(false);
  pane_->cursorLineHighlight(false);
  pane_->cursorLineUnderline(true);
  draw();
}

std::string Prompt::getInput() {
  for (;;) {
    portal::Event event;
    event.poll();
    pane_->clear();
    switch (event.type()) {
      case portal::Event::Type::select:
        return content_;
      case portal::Event::Type::keyBackspace:
        content_.pop_back();
        break;
      default:
        content_.push_back(event.character());
        break;
    }
    draw();
  }
}

void Prompt::setContent(const std::string& content) {
  content_ = content;
  draw();
}

void Prompt::draw() {
  pane_->print(content_);
  pane_->draw();
  refresh();
}

}
}
