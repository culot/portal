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
#include <sstream>
#include <vector>
#include <set>

#include <curses.h>

#include "popupwindow.h"
#include "inputwindow.h"
#include "gfx.h"
#include "ui.h"

namespace portal {

const std::string markerCategory("---");
const std::string markerFolded("-");
const std::string markerUnfolded("\\");

Ui::Ui() {
  filters_.set();
  gfx::Gfx::instance().init();
  createInterface();
  updatePanes();
}

Ui::~Ui() {
  gfx::Gfx::instance().terminate();
}

void Ui::display() {
  for (const auto& pane : pane_) {
    pane->draw();
  }
  tray_->display();
  gfx::Gfx::instance().update();
}

void Ui::handleEvent(const Event& event) {
  switch (event.type()) {
  case Event::Type::nextMode:
    selectNextMode();
    pane_[pkgList]->clearStatus();
    switch (currentMode_) {
    case Mode::browse:
      Pkg::instance().resetFilter();
      break;
    case Mode::search:
      applySearch();
      break;
    case Mode::filter:
      applyFilter();
      break;
    }
    updatePanes();
    updateTray();
    break;

  case Event::Type::select: {
    if (!Pkg::instance().isRepositoryEmpty()) {
      if (gotCategorySelected()) {
        std::string category = getSelectedItemName();
        toggleCategoryFolding(category);
        updatePanes();
      } else {
        registerPkgChange(event.type());
        updatePkgListPane();
      }
    }
    break;
  }

  case Event::Type::deselect:
    if (!Pkg::instance().isRepositoryEmpty()) {
      registerPkgChange(event.type());
      updatePkgListPane();
    }
    break;

  case Event::Type::keyUp:
  case Event::Type::keyDown:
    if (!Pkg::instance().isRepositoryEmpty()) {
      if (event.type() == Event::Type::keyUp) {
        pane_[pkgList]->moveCursorUp();
      } else if (event.type() == Event::Type::keyDown) {
        pane_[pkgList]->moveCursorDown();
      }
      updatePkgDescrPane();
    }
    break;

  case Event::Type::pageUp:
  case Event::Type::pageDown:
    if (!Pkg::instance().isRepositoryEmpty()) {
      if (event.type() == Event::Type::pageUp) {
        pane_[pkgDescr]->scrollUp();
      } else if (event.type() == Event::Type::pageDown) {
        pane_[pkgDescr]->scrollDown();
      }
      updatePkgDescrPane();
    }
    break;

  case Event::Type::character:
    switch (currentMode_) {
    case Mode::browse:
      // DO NOTHING
      break;
    case Mode::search:
      pane_[pkgList]->resetCursorPosition();
      promptSearch(event.character());
      applySearch();
      updatePanes();
      break;
    case Mode::filter:
      pane_[pkgList]->resetCursorPosition();
      promptFilter(event.character());
      applyFilter();
      updatePanes();
      break;
    }
    break;

  case Event::Type::go:
    if (!Pkg::instance().gotRootPrivileges()) {
      gfx::PopupWindow("Insufficient privileges, please retry as root",
                       gfx::PopupWindow::Type::warning);
    } else {
      performPending();
      closeAllFolds();
      updatePanes();
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
void Ui::createInterface() {
  int pkgPaneHeight = LINES * .6;
  int descrPaneHeight = LINES - pkgPaneHeight - 1;

  gfx::Size listSize, descrSize;
  listSize.setWidth(COLS);
  listSize.setHeight(pkgPaneHeight);
  descrSize.setWidth(COLS);
  descrSize.setHeight(descrPaneHeight);

  gfx::Point listPos, descrPos;
  descrPos.setY(pkgPaneHeight);

  gfx::Style listStyle;
  listStyle.borders = true;
  listStyle.highlight = true;

  pane_[pkgList] = std::unique_ptr<gfx::ScrollWindow>(new gfx::ScrollWindow(listSize, listPos));
  pane_[pkgList]->setStyle(listStyle);
  pane_[pkgDescr] = std::unique_ptr<gfx::ScrollWindow>(new gfx::ScrollWindow(descrSize, descrPos));

  gfx::Point trayPos;
  trayPos.setY(pkgPaneHeight);
  trayPos.setX(COLS / 2);
  tray_ = std::unique_ptr<gfx::Tray>(new gfx::Tray(trayPos, Mode::nbModes));
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
  gfx::Style rightAligned;
  rightAligned.align = gfx::Style::Alignment::right;
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
      pane_[pkgList]->print(pkgVersions, rightAligned);
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
    pane_[pkgDescr]->colorizeCurrentLine(gfx::Style::Color::cyan);

    std::string desc = Pkg::instance().getPkgAttr(origin, Pkg::Attr::description);
    std::stringstream commentStream(desc);
    std::string descLine;
    while (std::getline(commentStream, descLine, '\n')) {
      pane_[pkgDescr]->newline();
      pane_[pkgDescr]->print(descLine);
    }
  }
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
    case Event::Type::select:
      Pkg::instance().registerInstall(origin);
      break;

    case Event::Type::deselect:
      Pkg::instance().registerRemoval(origin);
      break;

    default:
      break;
    }
  }
}

void Ui::performPending() {
  busy_ = true;
  std::thread uiHint([this]() {busyStatus(*pane_[pkgList]);});
  uiHint.detach();

  std::thread pendingActions(&Pkg::performPending, &Pkg::instance());
  pendingActions.join();
  busy_ = false;
}

void Ui::promptFilter(int character) {
  switch (character) {
  case 'a':
    filters_.flip(Pkg::Statuses::available);
    break;
  case 'i':
    filters_.flip(Pkg::Statuses::installed);
    break;
  case 'p':
    filters_.flip(Pkg::Statuses::pendingInstall);
    break;
  case 'u':
    filters_.flip(Pkg::Statuses::upgradable);
    break;
  default:
    // DO NOTHING
    break;
  }
}

void Ui::applyFilter() const {
  Pkg::instance().applyFilter(filters_);

  static const std::string avlbLong = "(A)vailable";
  static const std::string instLong = "(I)nstalled";
  static const std::string pendLong = "(P)ending";
  static const std::string upgdLong = "(U)pgradable";
  static const std::string delimLong = " / ";
  static const std::string statusLong =
    avlbLong + delimLong + instLong + delimLong + pendLong + delimLong + upgdLong;

  static const std::string avlbShort = "A)vail";
  static const std::string instShort = "I)nst";
  static const std::string pendShort = "P)end";
  static const std::string upgdShort = "U)pgd";
  static const std::string delimShort = "/";
  static const std::string statusShort =
    avlbShort + delimShort + instShort + delimShort + pendShort + delimShort + upgdShort;

  std::string avlbStatus, instStatus, pendStatus, upgdStatus, statusString, delim;
  if ((pane_[pkgList]->size().width() - tray_->size().width()) / 2 < statusLong.length() + 8) {
    avlbStatus = avlbShort;
    instStatus = instShort;
    pendStatus = pendShort;
    upgdStatus = upgdShort;
    delim = delimShort;
    statusString = statusShort;
  } else {
    avlbStatus = avlbLong;
    instStatus = instLong;
    pendStatus = pendLong;
    upgdStatus = upgdLong;
    delim = delimLong;
    statusString = statusLong;
  }

  pane_[pkgList]->clearStatus();
  pane_[pkgList]->printStatus(statusString);
  gfx::Style unselectedStyle;
  unselectedStyle.color = gfx::Style::Color::black;
  unselectedStyle.bold = true;
  pane_[pkgList]->setStatusStyle(0, statusString.length(), unselectedStyle);

  gfx::Style selectedStyle;
  selectedStyle.color = gfx::Style::Color::cyan;
  int pos = 0;
  if (filters_[Pkg::Statuses::available]) {
    pane_[pkgList]->setStatusStyle(0, avlbStatus.length(), selectedStyle);
  }
  pos += avlbStatus.length() + delim.length();
  if (filters_[Pkg::Statuses::installed]) {
    pane_[pkgList]->setStatusStyle(pos, instStatus.length(), selectedStyle);
  }
  pos += instStatus.length() + delim.length();
  if (filters_[Pkg::Statuses::pendingInstall]) {
    pane_[pkgList]->setStatusStyle(pos, pendStatus.length(), selectedStyle);
  }
  pos += pendStatus.length() + delim.length();
  if (filters_[Pkg::Statuses::upgradable]) {
    pane_[pkgList]->setStatusStyle(pos, upgdStatus.length(), selectedStyle);
  }
}

void Ui::promptSearch(int character) {
  gfx::Point pos = gfx::Point::Label::center;
  gfx::InputWindow inputWindow(pos, 40);
  inputWindow.setContent(std::string(1, static_cast<char>(character)));
  searchString_ = inputWindow.getInput();
}

void Ui::applySearch() const {
  if (!searchString_.empty()) {
    Pkg::instance().search(searchString_);
    pane_[pkgList]->clearStatus();
    gfx::Style promptStyle;
    promptStyle.color = gfx::Style::Color::cyan;
    pane_[pkgList]->printStatus(searchString_, promptStyle);
  }
}

void Ui::busyStatus(gfx::ScrollWindow& pane) {
  pane.clearStatus();
  gfx::Style busyStyle;
  busyStyle.color = gfx::Style::Color::cyan;
  busyStyle.reverse = true;
  std::string busyString("     ");
  int busyStringLen = busyString.length();
  pane.printStatus(busyString);
  for (;;) {
    for (int x = 0; x < busyStringLen; ++x) {
      if (!busy_) {
        pane.clearStatus();
        pane.draw();
        gfx::Gfx::instance().update();
        return;
      } else {
        pane.setStatusStyle(0, busyStringLen, {});
        pane.setStatusStyle(x, 1, busyStyle);
        pane.draw();
        gfx::Gfx::instance().update();
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
      }
    }
  }
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

void Ui::selectNextMode() {
  ++currentMode_;
  if (currentMode_ == Mode::nbModes) {
    currentMode_ = 0;
  }
}

void Ui::updateTray() {
  tray_->selectSlot(currentMode_);
  std::thread popupModeName([this](){showCurrentModeName();});
  popupModeName.detach();
}

void Ui::showCurrentModeName() {
  gfx::Point center;
  center.setX(COLS / 2);
  center.setY(pane_[pkgList]->size().height() - 3);
  gfx::PopupWindow(modeName_[currentMode_], gfx::PopupWindow::Type::brief, center);
}

}
