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

#include <utility>
#include <algorithm>

#include "area.h"
#include "panel.h"

namespace gfx {

Panel::Panel() 
    : area_({0, 0}, Gfx::instance().getScreenSize()) {}

Panel::Panel(const Point & point, const Size & size)
    : area_(point, size) {}

void Panel::draw() {
  if (needRedraw_) {
    clear();
    drawContent();
    highlightRow(absCursorRowNum_);
    applyAttributes();
    drawScrollBar();
    drawBorder();
    drawTitle();
    drawStatus();

    needRedraw_ = false;
  }
}

void Panel::clear() {
  unsigned int xMin = area_.getXmin() + (drawBorder_ ? 1 : 0);
  unsigned int xMax = area_.getXmax() - (drawBorder_ ? 1 : 0);
  unsigned int yMin = area_.getYmin() + (drawBorder_ ? 1 : 0);
  unsigned int yMax = area_.getYmax() - (drawBorder_ ? 1 : 0);

  for (unsigned int y = yMin; y <= yMax; ++y)
    for (unsigned int x = xMin; x <= xMax; ++x)
      Gfx::instance().plot({x, y}, ' ');
}

void Panel::eraseContent() {
  dataGrid_.clear();
  rowAttrs_.clear();
}

void Panel::drawBorder() const {
  if (drawBorder_)
    area_.drawBorder();
}

void Panel::drawTitle() const {
  if (!title_.empty()) {
    unsigned int titleXmin = (area_.getWidth() - title_.length()) / 2 + area_.getXmin();
    unsigned int titleXmax = titleXmin + title_.length();
    Gfx::instance().plot({titleXmin - 2, area_.getYmin()}, SP_HORIZ_OPENG_BAR);
    Gfx::instance().plot({titleXmin - 1, area_.getYmin()}, ' ');
    Gfx::instance().plot({titleXmax, area_.getYmin()}, ' ');
    Gfx::instance().plot({titleXmax + 1, area_.getYmin()}, SP_HORIZ_CLOSEG_BAR);
    Gfx::instance().write({titleXmin, area_.getYmin()}, title_);
  }
}

void Panel::drawStatus() const {
  if (!status_.empty()) {
    unsigned long y = area_.getYmax();
    unsigned long x = area_.getXmin() + 1;
    while (x < area_.getXmax() - status_.length() - 6) {
      Gfx::instance().plot({x++, y}, SP_HORIZ_BAR);
    }
    Gfx::instance().plot({x++, y}, SP_HORIZ_OPENG_BAR);
    Gfx::instance().plot({x++, y}, ' ');
    Gfx::instance().write({x, y}, status_, ATTR_BOLD | statusColor_);
    x += status_.length();
    Gfx::instance().plot({x++, y}, ' ');
    Gfx::instance().plot({x++, y}, SP_HORIZ_CLOSEG_BAR);
    while (x < area_.getXmax() - 1) {
      Gfx::instance().plot({x++, y}, SP_HORIZ_BAR);
    }
  }
}

void Panel::drawScrollBar() const {
  if (drawScrollBar_) {
    unsigned long x = area_.getWidth() - 2;
    unsigned long yMin = area_.getYmin() + (drawBorder_ ? 1 : 0);
    unsigned long yMax = area_.getYmax() - (drawBorder_ ? 1 : 0);

    if (absFirstRowNum_ > 0)
      Gfx::instance().plot({x, yMin}, SP_SCROLL_ARROW_UP);

    if (dataGrid_.height() > 0 && getAbsLastRowNum() < dataGrid_.height())
      Gfx::instance().plot({x, yMax}, SP_SCROLL_ARROW_DOWN);
  }
}

void Panel::drawContent() {
  unsigned int x = area_.getXmin() + (drawBorder_ ? 1 : 0);
  unsigned int y = area_.getYmin() + (drawBorder_ ? 1 : 0);

  unsigned long lastRowNum = std::min(getAbsLastRowNum(), dataGrid_.height());
  for (unsigned long absRowNum = absFirstRowNum_;  absRowNum < lastRowNum; ++absRowNum)
    drawRowContent({x, y++}, absRowNum);
}

void Panel::drawRowContent(const Point& point, const unsigned int absRowNum) {
  unsigned int startCol = 0;

  for (size_t col = 0; col < dataGrid_.width(); ++col) {
    std::string content = dataGrid_.getValueAt({absRowNum, col});

    int colWidth = colWidth_.size() > col ? colWidth_[col] : -1;
    if (colWidth > 0)
      content.resize(colWidth, ' ');
    else
      content.append(" ");

    Gfx::instance().write({point.x + startCol, point.y}, content);
    startCol += content.length();
  }
}

void Panel::refreshStatus() {
  drawStatus();
  Gfx::instance().refresh();
}

void Panel::highlightRow(const unsigned int absRowNum) {
  if (highlightCurrent_)
    addRowAttributes(absRowNum, ATTR_REVERSE);
}

void Panel::applyAttributes() const {
  if (rowAttrs_.empty())
    return;

  unsigned int xMin = area_.getXmin() + (drawBorder_ ? 1 : 0);
  unsigned int xMax = area_.getXmax() - (drawBorder_ ? 1 : 0);
  unsigned int y = area_.getYmin() + (drawBorder_ ? 1 : 0);

  unsigned long lastRowNum = std::min(getAbsLastRowNum(), rowAttrs_.size());

  for (unsigned long rowNum = absFirstRowNum_; rowNum < lastRowNum; ++rowNum) {
    Attr fg = rowAttrs_[rowNum].first;
    Attr bg = rowAttrs_[rowNum].second;

    for (unsigned int x = xMin; x <= xMax; ++x)
      Gfx::instance().setAttributes({x, y}, fg, bg);

    ++y;
  }
}

void Panel::addRowAttributes(const unsigned int absRowNum, Attr fg, Attr bg) {
  resizeAttrsIfNeeded(absRowNum + 1);

  Attr attrFg = rowAttrs_[absRowNum].first | fg;
  Attr attrBg = rowAttrs_[absRowNum].second | bg;

  rowAttrs_[absRowNum] = std::pair<Attr, Attr>(attrFg, attrBg);
}

void Panel::removeRowAttributes(const unsigned long absRowNum, Attr fg, Attr bg) {
  Attr attrFg = rowAttrs_[absRowNum].first & ~fg;
  Attr attrBg = rowAttrs_[absRowNum].second & ~bg;

  rowAttrs_[absRowNum] = std::pair<Attr, Attr>(attrFg, attrBg);
}

void Panel::addRowColors(unsigned long absRowNum, Color fg, Color bg) {
  addRowAttributes(absRowNum, fg, bg);
}

Panel& Panel::status(const std::string& status, Color color) {
  status_ = status;
  statusColor_ = color;
  return *this;
}

Panel& Panel::layout(const Point& point, const Size& size) {
  area_.resetLayout(point, size);

  return *this;
}

Panel& Panel::content(portal::Grid<std::string>& content) {
  dataGrid_ = std::move(content);

  resizeAttrsIfNeeded(dataGrid_.height());

  return *this;
}

void Panel::setFixedColWidth(const std::vector<int>& width) {
  colWidth_ = width;
}

void Panel::handleEvent(portal::Event::Type event) {
  switch (event) {
    case portal::Event::Type::keyDown:
    case portal::Event::Type::keyShiftDown:
      moveCursor(Movement::down);
      break;

    case portal::Event::Type::keyUp:
    case portal::Event::Type::keyShiftUp:
      moveCursor(Movement::up);
      break;

    default:
      break;
  }
}

std::string Panel::getHighlightedRowContentAtCol(unsigned long colNum) {
  return dataGrid_.getValueAt({absCursorRowNum_, colNum});
}

std::string Panel::getContentAt(unsigned long row, unsigned long col) {
  return dataGrid_.getValueAt({row, col});
}

void Panel::movePage(Movement direction) {
  switch (direction) {
    case Movement::up:
      if (absFirstRowNum_ > 0) {
        --absFirstRowNum_;
        scroll(Movement::up);
      }
      break;

    case Movement::down:
      if (dataGrid_.height() > 0 && getAbsLastRowNum() < dataGrid_.height() - (drawBorder_ ? 1 : 0)) {
        absCursorRowNum_ = getAbsLastRowNum() + 1;
        scroll(Movement::down);
      }
      break;
  }
}

void Panel::moveCursor(Movement direction) {
  switch (direction) {
    case Movement::up:
      if (absCursorRowNum_ > 0) {
        --absCursorRowNum_;
        scroll(Movement::up);
        removeRowAttributes(absCursorRowNum_ + 1, ATTR_REVERSE);
        highlightRow(absCursorRowNum_);
      }
      break;

    case Movement::down:
      if (absCursorRowNum_ < dataGrid_.height() - 1) {
        ++absCursorRowNum_;
        scroll(Movement::down);
        removeRowAttributes(absCursorRowNum_ - 1, ATTR_REVERSE);
        highlightRow(absCursorRowNum_);
      }
      break;
  }
}

bool Panel::scroll(Movement direction) {
  bool scrollOk(false);

  switch (direction) {
    case Movement::up:
      if (absCursorRowNum_ < absFirstRowNum_) {
        --absFirstRowNum_;
        scrollOk = true;
      }
      break;

    case Movement::down:
      if (absCursorRowNum_ >= getAbsLastRowNum() - 1) {
        ++absFirstRowNum_;
        scrollOk = true;
      }
      break;
  }

  needRedraw_ = true;

  return scrollOk;
}

unsigned long Panel::getAbsRowNum(const unsigned int relRowNum) const {
  return relRowNum + absFirstRowNum_ - (drawBorder_ ? 1 : 0);
}

unsigned long Panel::getAbsLastRowNum() const {
  return getAbsRowNum(area_.getHeight());
}

void Panel::resizeAttrsIfNeeded(unsigned long size) {
  if (rowAttrs_.size() < size)
    rowAttrs_.resize(size, std::pair<Attr, Attr>(ATTR_NORMAL, ATTR_NORMAL));
}

}
