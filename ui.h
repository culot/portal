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
#include <vector>
#include <map>

#include "pkg.h"
#include "event.h"
#include "pane.h"
#include "tray.h"

namespace portal {

class Ui {
 public:
  static Ui&    instance() {static Ui instance_; return instance_;}

  void          display();
  void          handleEvent(const Event& event);

 private:
  Ui();
  ~Ui();
  Ui(const Ui&) = delete;
  void operator=(const Ui&) = delete;

  enum PaneType {
    pkgList,
    pkgDescr,
    nbtypes
  };

  enum pkgListItemType {
    category,
    pkg,
    nbPkgListItemTypes
  };

  struct pkgListItem {
    pkgListItemType type;
    std::string     name;
  };

  enum Mode {
    browse,
    search,
    filter,
//    dashboard,
    nbModes
  };

  std::string                    modeName_[nbModes] {"Browse", "Search", "Filter"};
  
  bool                           busy_ {false};

  std::unique_ptr<gfx::Pane>     pane_[PaneType::nbtypes];
  std::unique_ptr<gfx::Tray>     tray_;
  std::map<std::string, bool>    unfolded_;
  std::vector<pkgListItem>       pkgList_;
  int                            currentMode_ {Mode::browse};
  Pkg::Status                    filteringStatuses_;
  
  void                createInterface();
  void                updatePanes();
  void                buildPkgList();
  void                updatePkgListPane(const std::vector<std::string>& origins);
  void                updatePkgListPane();
  void                updatePkgDescrPane();
  const pkgListItem&  getCurrentPkgListItem() const;
  std::string         getSelectedItemName() const;
  bool                gotCategorySelected();
  bool                isCategory(const pkgListItem& str) const;
  void                toggleCategoryFolding(const std::string& category);
  void                closeAllFolds();
  void                registerPkgChange(Event::Type event);
  void                performPending();
  void                promptFilter(int character);
  void                promptSearch(int character) const;
  void                setBusyStatus(gfx::Pane& pane, const std::string& status);
  void                busyStatus(gfx::Pane& pane);
  void                warningStatus(gfx::Pane& pane, const std::string& status);
  bool                isCategoryFolded(const std::string& category) const;
  std::string         getStringForCategory(const std::string& category) const;
  std::string         getStringForPkg(const std::string& origin) const;
  std::string         getVersionsForPkg(const std::string& origin) const;
  void                selectNextMode();
  void                updateTray();
  void                showCurrentModeName();
};

}
