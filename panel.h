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
     
   0 /- - - -  +--------------+ 
   1 |         |              | 
   2 |         |              | 
   3 |         |______________| 
   4 |        /|             /|0 <- absFirstRowNum
   5 |       / |            / |1
   6 |      +--------------+  |2
   7 |      |  |           |  |3
   8 |      |  |           |  |4
   9 |      |  |           |  |5
  10 |      |  |___________|__|6 
  11 |      | /            | /| 
  12 |      |/             |/ | 
  13 |      +--------------+  | 
  14 |         |              | 
  15 |         |              | 
  16 |         |              | 
  17 \- - - -  +--------------+ 

 */

namespace gfx {

class Panel {
 public:
  enum class Movement {
    up,
    down 
  };

  Panel();
  Panel(const Point& point, const Size& size);

  void           draw();
  void           drawBorder(bool flag) {drawBorder_ = flag;}
  void           drawScrollBar(bool flag) {drawScrollBar_ = flag;}
  void           highlightCursorLine(bool flag) {highlightCurrent_ = flag;}
  void           resetCursor() {absCursorRowNum_ = 0;}
  void           clear();
  void           clearStatus() {status_.clear();}
  unsigned long  getWidth() const {return area_.getWidth();}
  unsigned long  getCurrentRowNum() const {return absCursorRowNum_;}
  void           eraseContent();
  void           requestRefresh() {needRedraw_ = true;}
  void           refreshStatus();
  Panel&         title(const std::string& title) {title_ = title; return *this;};
  Panel&         status(const std::string& status) {status_ = status; return *this;}
  Panel&         layout(const Point& point, const Size& size);
  Panel&         content(portal::Grid<std::string>& content);

  // Use the following method to set a fixed column width
  // if -1 or no column width is provided, then the column
  // width will fit the content length
  void           setFixedColWidth(const std::vector<int> & width);

  void           handleEvent(portal::Event::Type event);
  std::string    getHighlightedRowContentAtCol(unsigned long colNum);
  std::string    getContentAt(unsigned long row, unsigned long col);

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
  unsigned int              absFirstRowNum_ {0};
  unsigned int              absCursorRowNum_ {0};
  std::vector<int>          colWidth_;

  std::vector<std::pair<Attr, Attr>>   rowAttrs_;

  portal::Grid<std::string> dataGrid_;

  void            drawBorder() const;
  void            drawTitle() const;
  void            drawStatus() const;
  void            drawScrollBar() const;
  void            drawContent();
  void            drawRowContent(const Point& point, const unsigned int absRowNum);
  void            highlightRow(const unsigned int absRowNum);
  void            applyAttributes() const;
  void            movePage(Movement direction);
  void            moveCursor(Movement direction);
  bool            scroll(Movement direction);
  unsigned long   getAbsRowNum(const unsigned int relRowNum) const;
  unsigned long   getAbsLastRowNum() const;
  void            removeRowAttributes(const unsigned long absRowNum, Attr fg, Attr bg = ATTR_NORMAL);
  void            resizeAttrsIfNeeded(unsigned long size);
};

}
