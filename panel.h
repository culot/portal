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

#include <utility>
#include <string>
#include <vector>
#include <map>

#include "gfx.h"
#include "area.h"
#include "event.h"
#include "grid.h"

/*
     
             +--------------+ - - -\0
             |              |      |1
             |              |      |2
    v  _ _ _ |______________|      |3
    i |     /|             /|0     |4   <- absFirstRowNum = 4
    e      / |            / |1     |5
    w |   +--------------+  |2     |6
    p     |  |           |  |3     |7
    o |   |  |->cursor<- |  |4     |8   <- absCursorRowNum = 8
    r     |  |           |  |5     |9
    t |__ |_ |___________|__|6     |10
          | /            | /|      |11
          |/             |/ |      |12
          +--------------+  |      |13
             |              |      |14
             |              |      |15
             |              |      |16
             +--------------+ - - -/17  <- dataGrid_.height() - 1 = 17

 */

namespace gfx {

class Panel {
 public:
  enum class Movement {
    up,
    down 
  };

  Panel();
  Panel(Point point, Size size);

  void           draw();
  void           drawBorder(bool flag) {drawBorder_ = flag;}
  void           drawScrollBar(bool flag) {drawScrollBar_ = flag;}
  void           highlightCursorLine(bool flag) {highlightCurrent_ = flag;}
  void           resetPosition();
  void           clear();
  void           clearStatus() {status_.clear();}
  unsigned long  getWidth() const {return area_.getWidth();}
  unsigned long  getCurrentRowNum() const {return absCursorRowNum_;}
  void           eraseContent();
  void           requestRefresh() {needRedraw_ = true;}
  void           refreshStatus();
  Panel&         title(const std::string& title) {title_ = title; return *this;};
  Panel&         status(const std::string& status, Color color = gfx::color::DEFAULT);
  Panel&         layout(Point point, Size size);
  Panel&         content(portal::Grid<std::string>& content);

  // Use the following method to set a fixed column width
  // if -1 or no column width is provided, then the column
  // width will fit the content length
  void           setFixedColWidth(const std::vector<int>& width);

  void           handleEvent(portal::Event::Type event);
  std::string    getHighlightedRowContentAtCol(unsigned long colNum);
  std::string    getContentAt(unsigned long row, unsigned long col);
  std::string    getStatus() const {return status_;}

  void           addRowColors(unsigned long absRowNum, Color fg, Color bg = ATTR_NORMAL);
  void           addRowAttributes(const unsigned int absRowNum, Attr fg, Attr bg = ATTR_NORMAL);

 private:
  bool                      needRedraw_ {true};
  bool                      drawBorder_ {false};
  bool                      drawScrollBar_ {true};
  bool                      highlightCurrent_ {false};

  Area                      area_;
  std::string               title_;
  std::string               status_;
  Color                     statusColor_ {gfx::color::DEFAULT};
  unsigned long             absFirstRowNum_ {0};
  unsigned long             absCursorRowNum_ {0};
  std::vector<int>          colWidth_;

  std::vector<std::pair<Attr, Attr>>   rowAttrs_;

  portal::Grid<std::string> dataGrid_;

  void            drawBorder() const;
  void            drawTitle() const;
  void            drawStatus() const;
  void            drawScrollBar() const;
  void            drawContent();
  void            drawRowContent(Point point, unsigned int absRowNum);
  void            highlightRow(unsigned int absRowNum);
  void            applyAttributes() const;
  void            movePage(Movement direction);
  void            moveCursor(Movement direction);
  bool            scroll(Movement direction);
  unsigned long   getAbsRowNum(unsigned int relRowNum) const;
  unsigned long   getAbsLastRowNum() const;
  void            removeRowAttributes(unsigned long absRowNum, Attr fg, Attr bg = ATTR_NORMAL);
  void            resizeAttrsIfNeeded(unsigned long size);
};

}
