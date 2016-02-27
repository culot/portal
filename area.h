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

#include "gfx.h"

namespace gfx {

enum class Corner {
  topLeft,
  topRight,
  bottomLeft,
  bottomRight
};

class Area {
 public:
  Area() {}
  Area(Point point, Size size);

  Area&  resetLayout(Point point, Size size);
  Area&  size(Size size);
  void   drawBorder() const;

  Point         getCornerPoint(Corner corner) const;
  Size          getSize() const {return size_;}
  unsigned int  getHeight() const {return size_.height;}
  unsigned int  getWidth() const {return size_.width;}
  unsigned int  getXmin() const {return topLeft_.x;}
  unsigned int  getXmax() const {return bottomRight_.x;}
  unsigned int  getYmin() const {return topLeft_.y;}
  unsigned int  getYmax() const {return bottomRight_.y;}

 private:
  Point   topLeft_ {0, 0};
  Point   bottomRight_ {0, 0};
  Size    size_ {0, 0};

  void  normalize();
  bool  hasMergedCorners() const;
  bool  hasEmptySize() const;
};

}
