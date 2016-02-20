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

#include "gfx.h"
#include "pkg.h"
#include "area.h"
#include "grid.h"
#include "form.h"

#include "ui.h"

namespace portal {

const std::string markerCategory("---");
const std::string markerFolded("-");
const std::string markerUnfolded("\\");

Ui::Ui() {
  setPanelsLayout();
  updatePanels();
}

void Ui::display() {
  for (auto& panel : panels_)
    panel.draw();

  gfx::Gfx::instance().refresh();
}

Event::Type Ui::poll() const {
  Event ev;
  return ev.poll();
}

void Ui::handleEvent(Event::Type event) {
  switch (event) {
    case Event::Type::select: {
      if (gotCategorySelected())
      {
        std::string category = getSelectedCategoryName();
        toggleCategoryFolding(category);
        updatePanels();
      }
      break;
    }

    case Event::Type::keyUp:
    case Event::Type::keyDown:
      panels_[pkgList].handleEvent(event);
      updatePkgCommentPanel();
      updatePkgDescrPanel();
      break;

    case Event::Type::keyShiftUp:
    case Event::Type::keyShiftDown:
      panels_[pkgDescr].handleEvent(event);
      updatePkgDescrPanel();
      break;

    case Event::Type::flagInstall:
    case Event::Type::flagRemove:
      registerPkgChange(event);
      updatePkgListPanel();
      break;

    case Event::Type::filter:
      panels_[pkgList].status("filter");
      panels_[pkgList].refreshStatus();
      panels_[pkgList].resetCursor();
      promptFilter();
      updatePanels();
      break;

    case Event::Type::search:
      panels_[pkgList].status("search");
      panels_[pkgList].refreshStatus();
      panels_[pkgList].resetCursor();
      promptSearch();
      updatePanels();
      break;

    case Event::Type::go:
      if (!Pkg::instance().gotRootPrivileges()) {
        warningStatus(panels_[pkgList], "Insufficient privileges, please retry as root");
      } else {
        performPending();
        closeAllFolds();
        updatePkgListPanel();
      }
      break;

    case Event::Type::redraw:
      gfx::Gfx::instance().reinit();
      for (auto& panel : panels_)
        panel.requestRefresh();
      break;

    default:
      // forward the event to all panels
      for (auto & panel : panels_)
        panel.handleEvent(event);
      break;
  }
}

void Ui::setPanelsLayout() {
  unsigned int screenHeight = gfx::Gfx::instance().getScreenSize().height;
  unsigned int screenWidth = gfx::Gfx::instance().getScreenSize().width;

  unsigned int pkgPanelHeight = screenHeight * .6;
  unsigned int commentPanelHeight = 1;
  unsigned int descrPanelHeight = screenHeight - pkgPanelHeight - commentPanelHeight;

  gfx::Size pkgPanelSize = {pkgPanelHeight, screenWidth};
  gfx::Size commentPanelSize = {commentPanelHeight, screenWidth};
  gfx::Size descrPanelSize = {descrPanelHeight, screenWidth};

  panels_[pkgList].layout({0, 0}, pkgPanelSize);
  panels_[pkgList].drawBorder(true);

  panels_[pkgComment].layout({0, pkgPanelHeight}, commentPanelSize);

  panels_[pkgDescr].layout({0, pkgPanelHeight + commentPanelHeight}, descrPanelSize);
}

void Ui::updatePanels() {
  updatePkgListPanel();
  updatePkgCommentPanel();
  updatePkgDescrPanel();
}

// Build a hierarchy of categories and ports:
//
// ---- category1
// ---\ category2
// -    port1
// +    port2
// ---- category3
void Ui::updatePkgListPanel() {
  Grid<std::string> dataGrid(DataColumn::nbDataColumns);

  // Only set a fixed width for the first column (category marker or port status)
  panels_[pkgList].setFixedColWidth({static_cast<int>(markerCategory.length() + 2)});

  panels_[pkgList].eraseContent();

  std::vector<std::string> categories = Pkg::instance().getPkgCategories();
  for (const auto & category : categories) {
    std::string categorySize = "("
        + std::to_string(Pkg::instance().getCategorySize(category))
        + ")";

    if (unfolded_.find(category) != unfolded_.end() && unfolded_[category] == true) {
      dataGrid.set(DataColumn::categoryMarker, markerCategory + markerUnfolded);
      dataGrid.set(DataColumn::categoryName, category);
      dataGrid.set(DataColumn::categorySize, categorySize);

      std::vector<std::string> origins = Pkg::instance().getPkgOrigins(category);
      for (const auto& origin : origins) {
        dataGrid.addRow();
        Pkg::Status current = Pkg::instance().getCurrentStatus(origin); 
        std::string status = Pkg::instance().getStatusAsString(current);

        Pkg::Status pending = Pkg::instance().getPendingStatus(origin);
        if (pending != Pkg::Status::unset) {
          panels_[pkgList].addRowAttributes(dataGrid.height() - 1, gfx::ATTR_BOLD);
          status.append("[");
          status.append(Pkg::instance().getStatusAsString(pending));
          status.append("]");
        }

        dataGrid.set(DataColumn::portStatus, status);
        dataGrid.set(DataColumn::portName, Pkg::instance().getNameFromOrigin(origin));
      }
    } else {
      dataGrid.set(DataColumn::categoryMarker, markerCategory + markerFolded);
      dataGrid.set(DataColumn::categoryName, category);
      dataGrid.set(DataColumn::categorySize, categorySize);
    }

    dataGrid.addRow();
  }

  dataGrid.removeRow();
  panels_[pkgList].content(dataGrid);
  panels_[pkgList].highlightCursorLine(true);
  panels_[pkgList].requestRefresh();
}

void Ui::updatePkgCommentPanel() {
  panels_[pkgComment].setFixedColWidth(std::vector<int>(panels_[pkgComment].getWidth() - 2));
  panels_[pkgComment].drawScrollBar(false);
  panels_[pkgComment].eraseContent();

  Grid<std::string> dataGrid(1);

  if (!gotCategorySelected()) {
    std::string origin = getSelectedPortOrigin();
    std::string comment = Pkg::instance().getPkgAttr(origin, Pkg::Attr::comment);

    dataGrid.set(DataColumn::portComment, comment);
  }

  panels_[pkgComment].content(dataGrid);
  panels_[pkgComment].addRowColors(0, gfx::COLOR_DEFAULT, gfx::COLOR_BLUE);
  panels_[pkgComment].requestRefresh();
}

void Ui::updatePkgDescrPanel() {
  panels_[pkgDescr].setFixedColWidth(std::vector<int>(panels_[pkgList].getWidth() - 2));
  panels_[pkgDescr].eraseContent();

  if (!gotCategorySelected()) {
    Grid<std::string> dataGrid(1);

    std::string origin = getSelectedPortOrigin();
    std::string desc = Pkg::instance().getPkgAttr(origin, Pkg::Attr::description);

    std::stringstream commentStream(desc);
    std::string descLine;
    while (std::getline(commentStream, descLine, '\n')) {
      dataGrid.set(DataColumn::portDescr, descLine);
      dataGrid.addRow();
      descLine.clear();
    }

    dataGrid.removeRow();
    panels_[pkgDescr].content(dataGrid);
  }

  panels_[pkgDescr].requestRefresh();
}

std::string Ui::getSelectedCategoryName() {
  std::string category = panels_[pkgList].getHighlightedRowContentAtCol(DataColumn::categoryName);

  return category;
}

std::string Ui::getSelectedPortOrigin() {
  std::string name = getSelectedPortName();
  std::string category = getSelectedPortCategory();

  return std::string(category + "/" + name);
}

std::string Ui::getSelectedPortName() {
  std::string name = panels_[pkgList].getHighlightedRowContentAtCol(DataColumn::portName);

  return name;
}

std::string Ui::getSelectedPortCategory() {
  for (long rowNum = panels_[pkgList].getCurrentRowNum(); rowNum >= 0; --rowNum)
    if (isCategory(panels_[pkgList].getContentAt(rowNum, DataColumn::categoryMarker)))
      return panels_[pkgList].getContentAt(rowNum, DataColumn::categoryName);

  throw std::runtime_error("Ui::getSelectedPortCategory(): Unable to find category for port ["
                           + getSelectedPortName()
                           + "]");
}

bool Ui::gotCategorySelected() {
  std::string selected = panels_[pkgList].getHighlightedRowContentAtCol(DataColumn::categoryMarker);

  return isCategory(selected);
}

bool Ui::isCategory(const std::string& str) const {
  return str.find(markerCategory) != std::string::npos;
}

void Ui::toggleCategoryFolding(const std::string& category) {
  if (unfolded_.find(category) != unfolded_.end())
    unfolded_[category] = unfolded_[category] == true ? false : true;
  else
    unfolded_[category] = true;
}

void Ui::closeAllFolds() {
  unfolded_.clear();
  panels_[pkgList].resetCursor();
}

void Ui::registerPkgChange(Event::Type event) {
  if (!gotCategorySelected()) {
    std::string origin = getSelectedPortOrigin();

    switch (event) {
      case Event::Type::flagInstall:
        Pkg::instance().registerPending(origin, Pkg::Status::installed);
        break;

      case Event::Type::flagRemove:
        Pkg::instance().registerPending(origin, Pkg::Status::uninstalled);
        break;

      default:
        break;
    }
  }
}

void Ui::performPending() {
  busy_ = true;
  std::thread uiHint([this]() {busyStatus(panels_[pkgList]);});
  uiHint.detach();

  std::thread pendingActions(&Pkg::performPending, &Pkg::instance());
  pendingActions.join();
  busy_ = false;
  panels_[pkgList].clearStatus();
}

void Ui::promptFilter() {
  Pkg::Status status = Pkg::Status::unset;
  panels_[pkgList].clearStatus();

  portal::Event ev;
  portal::Event::Type evType;
  char c;

  std::tie(evType, c) = ev.getRawInput();
  switch (evType) {
    case portal::Event::Type::keyPressed:
      switch (c) {
        case 'i':
        case '+':
          status = Pkg::Status::installed;
          panels_[pkgList].status("Installed");
          break;

        case '-':
        case 'a':
          status = Pkg::Status::uninstalled;
          panels_[pkgList].status("Available");
          break;
      }
      break;

    default:
      break;
  } 

  Pkg::instance().filter(status);
}

void Ui::promptSearch() const {
  gfx::Form form;
  form.label(" Input search pattern ");
  form.draw();
  std::string query = form.getInput();
  form.erase();

  Pkg::instance().search(query);
}

void Ui::setBusyStatus(gfx::Panel& panel, const std::string& status) {
  panel.status(status);
  panel.refreshStatus();
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

void Ui::busyStatus(gfx::Panel& panel) {
  while (busy_) {
    setBusyStatus(panel, "Please wait.  ");
    if (!busy_)
      return;
    setBusyStatus(panel, "Please wait.. ");
    if (!busy_)
      return;
    setBusyStatus(panel, "Please wait...");
    if (!busy_)
      return;
    setBusyStatus(panel, "Please wait ..");
    if (!busy_)
      return;
    setBusyStatus(panel, "Please wait  .");
    if (!busy_)
      return;
  }
}

void Ui::warningStatus(gfx::Panel& panel, const std::string& status) {
  std::string oldStatus = panel.getStatus();
  panel.status(status, gfx::COLOR_RED);
  panel.refreshStatus();
  std::this_thread::sleep_for(std::chrono::milliseconds(1500));
  panel.status(oldStatus);
  panel.requestRefresh();
}

}
