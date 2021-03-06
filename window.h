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

#include <memory>
#include <string>

#include "gfx.h"

namespace portal {
namespace gfx {


class Window {
 public:
  Window();
  Window(const Size& size, const Point& pos, const Style& style = {});
  ~Window();

  void   setSize(const Size& size);
  void   setPosition(const Point& pos);
  void   setStyle(const Style& style);
  Size   size() const;
  Point  position() const;
  Style  style() const;

  virtual void   print(const std::string& msg, const Style& style = {});
  virtual void   print(int c, const Point& pos = {}, const Style& style = {}) const;
  virtual void   printStatus(const std::string& status,
                             const Style& style = {}) const;
  virtual void   setStatusStyle(int xpos,
                                int len,
                                const Style& style) const;
  virtual void   clearStatus() const;
  virtual void   draw() const;
  virtual void   clear();

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;

  Window(const Window&) = delete;
  void operator=(const Window&) = delete;
};

}
}
