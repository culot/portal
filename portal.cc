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

#include <syslog.h>
#include <unistd.h>

#include <stdexcept>
#include <iostream>

#include "gfx.h"
#include "ui.h"
#include "event.h"
#include "pkg.h"

using namespace portal;

void usage(void) {
  std::cerr << "usage: portal [-a]" << std::endl;
  exit(1);
}

int main(int argc, char** argv) {
  int opt;
  while ((opt = getopt(argc, argv, "a")) != -1) {
    switch (opt) {
      case 'a':
        gfx::useAsciiOnly();
        break;

      default:
        usage();
    }
  }

  Pkg::instance().reload();
  Ui::instance().display();

  try {
    Event::Type event;
    while ((event = Ui::instance().poll()) != Event::Type::quit) {
      Ui::instance().handleEvent(event);
      Ui::instance().display();
    }
  }
  catch (std::exception & e) {
    syslog(LOG_ERR, "%s", e.what());
  }

  return 0;
}
