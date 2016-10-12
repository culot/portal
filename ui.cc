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

#include <thread>
#include <chrono>
#include <stdexcept>
#include <sstream>
#include <vector>
#include <set>

#include <curses.h>

#include "pkg.h"
#include "point.h"
#include "size.h"
#include "ui.h"

namespace portal {

const std::string markerCategory("---");
const std::string markerFolded("-");
const std::string markerUnfolded("\\");

Ui::Ui() {
  initscr();

  // XXX Need more elegant color definitions (enum?)
  // XXX Need to deal with B&W terminals
  if (has_colors() && start_color() == OK) {
    init_pair(1, COLOR_CYAN, COLOR_BLUE);
  } else {
    throw std::runtime_error("Sorry, B&W terminals not supported yet");
  }

  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  curs_set(0);
  refresh();  // A refresh might seem unnecessary here, but user input is
              // gathered from stdscr via a call to getch, which does an
              // implicit refresh first (don't ask me why...). Hence doing
              // this refresh explicitly avoids a black screen when portal
              // starts. The black screen does not appear afterwards as
              // stdscr is never touched by portal, so curses detects it
              // does not need any subsequent refreshes.
  createPanes();
  updatePanes();
}

Ui::~Ui() {
  clear();
  endwin();
}

void Ui::display() {
  for (auto& pane : pane_) {
    pane->draw();
  }
  doupdate();
}

void Ui::handleEvent(Event::Type event) {
  switch (event) {
    case Event::Type::select: {
      if (!Pkg::instance().isRepositoryEmpty() && gotCategorySelected()) {
        std::string category = getSelectedItemName();
        toggleCategoryFolding(category);
        updatePanes();
      }
      break;
    }

    case Event::Type::keyUp:
    case Event::Type::keyDown:
    case Event::Type::pageUp:
    case Event::Type::pageDown:
      if (!Pkg::instance().isRepositoryEmpty()) {
        if (event == Event::Type::keyUp) {
          pane_[pkgList]->moveCursorUp();
        } else if (event == Event::Type::keyDown) {
          pane_[pkgList]->moveCursorDown();
        }
        // XXX Implement scroll up and down
        updatePkgDescrPane();
      }
      break;

    case Event::Type::keyShiftUp:
    case Event::Type::keyShiftDown:
      if (!Pkg::instance().isRepositoryEmpty()) {
        if (event == Event::Type::keyShiftUp) {
          pane_[pkgDescr]->moveCursorUp();
        } else if (event == Event::Type::keyShiftDown) {
          pane_[pkgDescr]->moveCursorDown();
        }
        updatePkgDescrPane();
      }
      break;

    case Event::Type::flagInstall:
    case Event::Type::flagRemove:
      if (!Pkg::instance().isRepositoryEmpty()) {
        registerPkgChange(event);
        updatePkgListPane();
      }
      break;

    case Event::Type::filter:
      /* XXX Implement
      pane_[pkgList]->status("filter");
      pane_[pkgList]->refreshStatus();
      pane_[pkgList]->resetPosition();
      promptFilter();
      updatePanes();
      */
      break;

    case Event::Type::search:
      /* Implement
      pane_[pkgList]->status("search");
      pane_[pkgList]->refreshStatus();
      pane_[pkgList]->resetPosition();
      promptSearch();
      updatePanes();
      */
      break;

    case Event::Type::go:
      if (!Pkg::instance().gotRootPrivileges()) {
//        warningStatus(pane_[pkgList], "Insufficient privileges, please retry as root");
      } else {
        performPending();
        closeAllFolds();
        updatePkgListPane();
      }
      break;

    case Event::Type::redraw:
      display();
      break;

    default:
      break;
  }
}

/*
    +------------------------------+
    |                              | \
    |                              | |
    |                              | | pane_[pkgList]
    |                              | |
    |                              | /
    +------------------------------+
    |                              | \ <-- comment
    |                              | | 
    |                              | / pane_[pkgDescr]
    +------------------------------+ 
 */
void Ui::createPanes() {
  int pkgPaneHeight = LINES * .6;
  int descrPaneHeight = LINES - pkgPaneHeight - 1;

  gfx::Size listSize, descrSize;
  listSize.setWidth(COLS);
  listSize.setHeight(pkgPaneHeight);
  descrSize.setWidth(COLS);
  descrSize.setHeight(descrPaneHeight);

  gfx::Point listPos, descrPos;
  descrPos.setY(pkgPaneHeight);

  pane_[pkgList] = std::unique_ptr<gfx::Pane>(new gfx::Pane(listSize, listPos));
  pane_[pkgDescr] = std::unique_ptr<gfx::Pane>(new gfx::Pane(descrSize, descrPos));
  pane_[pkgDescr]->borders(false);
  pane_[pkgDescr]->cursorLineHighlight(false);
}

void Ui::updatePanes() {
  if (Pkg::instance().isRepositoryEmpty()) {
    for (auto& pane : pane_) {
      pane->clear();
      pane->resetCursorPosition();
    }
  } else {
    updatePkgListPane();
    updatePkgDescrPane();
  }
}

void Ui::buildPkgList() {
  pkgList_.clear();
  std::vector<std::string> categories = Pkg::instance().getPkgCategories();
  for (const auto& category : categories) {
    pkgList_.push_back({pkgListItemType::category, category});
    if (!isCategoryFolded(category)) {
      std::vector<std::string> origins = Pkg::instance().getPkgOrigins(category);
      for (const auto& origin : origins) {
        pkgList_.push_back({pkgListItemType::pkg, origin});
      }
    }
  }
}

// Build a hierarchy of categories and ports:
//
// ---- category1
// ---\ category2
// -    port1
// +    port2
// ---- category3
void Ui::updatePkgListPane() {
  buildPkgList();
  pane_[pkgList]->clear();
  for (const auto& item : pkgList_) {
    switch (item.type) {
    case pkgListItemType::category: {
      std::string categoryString = getStringForCategory(item.name);
      pane_[pkgList]->print(categoryString);
    }
    break;
    case pkgListItemType::pkg: {
      std::string pkgString = getStringForPkg(item.name);
      pane_[pkgList]->print(pkgString);
      std::string pkgVersions = getVersionsForPkg(item.name);
      pane_[pkgList]->print(pkgVersions, gfx::Pane::Align::right);
      //if (Pkg::instance().hasPendingActions(item.name)) {
        // XXX change attributes
        //pane_[pkgList].addRowAttributes(dataGrid.height() - 1, gfx::ATTR_BOLD);
      //}
    }
    break;
    default:
      break;
    }
    pane_[pkgList]->newline();
  }
}

void Ui::updatePkgDescrPane() {
  pane_[pkgDescr]->clear();

  if (!gotCategorySelected()) {
    std::string origin = getSelectedItemName();
    std::string comment = Pkg::instance().getPkgAttr(origin, Pkg::Attr::comment);
    pane_[pkgDescr]->print(comment);
    pane_[pkgDescr]->colorizeCurrentLine(1);
    
    std::string desc = Pkg::instance().getPkgAttr(origin, Pkg::Attr::description);
    std::stringstream commentStream(desc);
    std::string descLine;
    while (std::getline(commentStream, descLine, '\n')) {
      pane_[pkgDescr]->newline();
      pane_[pkgDescr]->print(descLine);
    }
  }

  //pane_[pkgDescr].requestRefresh();
}

const Ui::pkgListItem& Ui::getCurrentPkgListItem() const {
  int index = pane_[pkgList]->getCursorRowNum();
  return pkgList_[index];
}

std::string Ui::getSelectedItemName() const {
  const pkgListItem& item = getCurrentPkgListItem();
  return item.name;
}

bool Ui::gotCategorySelected() {
  const pkgListItem& item = getCurrentPkgListItem();
  return isCategory(item);
}

bool Ui::isCategory(const pkgListItem& item) const {
  return item.type == pkgListItemType::category;
}
  
void Ui::toggleCategoryFolding(const std::string& category) {
  if (unfolded_.find(category) != unfolded_.end()) {
    unfolded_[category] = unfolded_[category] == true ? false : true;
  } else {
    unfolded_[category] = true;
  }
}

void Ui::closeAllFolds() {
  unfolded_.clear();
  pane_[pkgList]->resetCursorPosition();
}

void Ui::registerPkgChange(Event::Type event) {
  if (!gotCategorySelected()) {
    std::string origin = getSelectedItemName();

    switch (event) {
      case Event::Type::flagInstall:
        Pkg::instance().registerInstall(origin);
        break;

      case Event::Type::flagRemove:
        Pkg::instance().registerRemoval(origin);
        break;

      default:
        break;
    }
  }
}

void Ui::performPending() {
  busy_ = true;
  // XXX implement busy status
//  std::thread uiHint([this]() {busyStatus(pane_[pkgList]);});
//  uiHint.detach();

  std::thread pendingActions(&Pkg::performPending, &Pkg::instance());
  pendingActions.join();
  busy_ = false;
//  pane_[pkgList].clearStatus();
}

void Ui::promptFilter() {
//  pane_[pkgList].clearStatus();

  portal::Event ev;
  portal::Event::Type evType;
  char c;

  std::tie(evType, c) = ev.getRawInput();
  switch (evType) {
    case portal::Event::Type::keyPressed:
      switch (c) {
        case 'n':
          Pkg::instance().resetFilter();
          break;

        case 'i':
        case '+':
//          pane_[pkgList].status("Installed");
          Pkg::instance().filterInstalled();
          break;

        case '-':
        case 'a':
//          pane_[pkgList].status("Available");
          Pkg::instance().filterAvailable();
          break;

        case 'p':
//          pane_[pkgList].status("Pending");
          Pkg::instance().filterPending();
          break;

        case 'u':
//          pane_[pkgList].status("Upgradable");
          Pkg::instance().filterUpgradable();
          break;
      }
      break;

    default:
      break;
  }
}

  // XXX implement prompt
void Ui::promptSearch() const {
  /*
  gfx::Form form;
  form.label(" Input search pattern ");
  form.draw();
  std::string query = form.getInput();
  form.erase();

  Pkg::instance().search(query);
  */
}

  // XXX implement busy status
void Ui::setBusyStatus(gfx::Pane& pane, const std::string& status) {
  /*
  pane.status(status);
  pane.refreshStatus();
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  */
}

void Ui::busyStatus(gfx::Pane& pane) {
  /*
  while (busy_) {
    setBusyStatus(pane, "Please wait.  ");
    if (!busy_)
      return;
    setBusyStatus(pane, "Please wait.. ");
    if (!busy_)
      return;
    setBusyStatus(pane, "Please wait...");
    if (!busy_)
      return;
    setBusyStatus(pane, "Please wait ..");
    if (!busy_)
      return;
    setBusyStatus(pane, "Please wait  .");
    if (!busy_)
      return;
  }
  */
}

void Ui::warningStatus(gfx::Pane& pane, const std::string& status) {
  /*
  std::string oldStatus = pane.getStatus();
  pane.status(status, gfx::color::RED);
  pane.refreshStatus();
  std::this_thread::sleep_for(std::chrono::milliseconds(1500));
  pane.status(oldStatus);
  pane.requestRefresh();
  */
}

bool Ui::isCategoryFolded(const std::string& category) const {
  return unfolded_.find(category) == unfolded_.end() || unfolded_.at(category) == false;
}

std::string Ui::getStringForCategory(const std::string& category) const {
  std::string categoryString(markerCategory);
  if (isCategoryFolded(category)) {
    categoryString.append(markerFolded);
  } else {
    categoryString.append(markerUnfolded);
  }
  categoryString.append(" ");
  categoryString.append(category);
  categoryString.append(" (");
  categoryString.append(std::to_string(Pkg::instance().getCategorySize(category)));
  categoryString.append(")");

  return categoryString;
}

std::string Ui::getStringForPkg(const std::string& origin) const {
  std::string pkgString = Pkg::instance().getCurrentStatusAsString(origin);
  if (Pkg::instance().hasPendingActions(origin)) {
    pkgString.append("[");
    pkgString.append(Pkg::instance().getPendingStatusAsString(origin));
    pkgString.append("] ");
  } else if (Pkg::instance().isUpgradable(origin)) {
    pkgString.append("[^] ");
  } else {
    pkgString.append("    ");
  }
  pkgString.append(Pkg::instance().getNameFromOrigin(origin));

  return pkgString;
}

std::string Ui::getVersionsForPkg(const std::string& origin) const {
  std::string pkgVersions = Pkg::instance().getLocalVersion(origin);
  if (!pkgVersions.empty()) {
    pkgVersions.append("    ");
  }
  pkgVersions.append(Pkg::instance().getRemoteVersion(origin));

  return pkgVersions;
}

}
