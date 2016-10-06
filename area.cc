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

#include "area.h"

namespace portal {
namespace gfx {

Area::Area(Point point, Size size)
    : topLeft_(point),
      size_(size) {
  normalize();
}

Area& Area::resetLayout(Point point, Size size) {
  topLeft_ = point;
  bottomRight_ = {0, 0};
  size_ = size;
  normalize();

  return *this;
}

Area& Area::size(Size size) {
  size_ = size;
  normalize();

  return *this;
}

void Area::drawBorder() const {
  unsigned int xMin = getXmin();
  unsigned int xMax = getXmax();
  unsigned int yMin = getYmin();
  unsigned int yMax = getYmax();

  for (unsigned int x = xMin; x <= xMax; ++x) {
    Gfx::instance().plot({x, yMin}, SP_HORIZ_BAR);
    Gfx::instance().plot({x, yMax}, SP_HORIZ_BAR);
  }

  for (unsigned int y = yMin; y <= yMax; ++y) {
    Gfx::instance().plot({xMin, y}, SP_VERT_BAR);
    Gfx::instance().plot({xMax, y}, SP_VERT_BAR);
  }

  Gfx::instance().plot({xMin, yMin}, SP_TOP_LEFT);
  Gfx::instance().plot({xMax, yMin}, SP_TOP_RIGHT);
  Gfx::instance().plot({xMin, yMax}, SP_BOT_LEFT);
  Gfx::instance().plot({xMax, yMax}, SP_BOT_RIGHT);
}

Point Area::getCornerPoint(Corner corner) const {
  switch (corner) {
    case Corner::topLeft:
      return topLeft_;
    case Corner::topRight:
      return {bottomRight_.x, topLeft_.y};
    case Corner::bottomLeft:
      return {topLeft_.x, bottomRight_.y};
    case Corner::bottomRight:
      return bottomRight_;
  }
}

void Area::normalize() {
  if (hasEmptySize()) {
    if (!hasMergedCorners()) {
      int width = bottomRight_.x - topLeft_.x + 1;
      int height = bottomRight_.y - topLeft_.y + 1;

      if (height < 0 || width < 0)
        throw std::runtime_error("Area::normalize(): invalid size in area definition");

      size_.height = height;
      size_.width  = width;
    }
  } else {
    // Check which corner was set
    if (bottomRight_.y != 0 && bottomRight_.x != 0) {
      int x = size_.width - bottomRight_.x - 1;
      int y = size_.width - bottomRight_.y - 1;

      if (y < 0 || x < 0)
        throw std::runtime_error("Area::normalize(): invalid bottom right point or size in area definition");

      topLeft_.y = y;
      topLeft_.x = x;
    } else {
      int x = size_.width + topLeft_.x - 1;
      int y = size_.height + topLeft_.y - 1;

      if (y < 0 || x < 0)
        throw std::runtime_error("Area::normalize(): invalid top left point or size in area definition");

      bottomRight_.y = y;
      bottomRight_.x = x;
    }
  }
}

bool Area::hasMergedCorners() const {
  return (topLeft_.x == bottomRight_.x) && (topLeft_.y == bottomRight_.y);
}

bool Area::hasEmptySize() const {
  return (size_.width == 0) && (size_.height == 0);
}

}
}
