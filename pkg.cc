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

#include <unistd.h>
#include <sys/types.h>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

#include "pkg.h"

namespace portal {

static const char delimiter = '\2';

Pkg::Pkg() {
  checkPrivileges();
}

void Pkg::checkPrivileges() {
  rootPrivileges_ = getuid() ? false : true;
}

void Pkg::reload(Repo repo) {
  switchToReferenceRepository();

  switch (repo) {
    case Repo::all:
      buildPackagesList(Repo::remote);
      buildPackagesList(Repo::local);
      break;

    default:
      buildPackagesList(repo);
      break;
  }
}

void Pkg::buildPackagesList(Repo repo) {
  std::vector<Port> pkgs;
  switch (repo) {
    case Repo::local: {
      std::stringstream args;
      args << "query -a '%o"
           << delimiter
           << "%v"
           << delimiter
           << "%c"
           << delimiter
           << "%e"
           << delimiter
           << "'";
      pkgs = runPkg(args.str());
      fillPkgRepo(Repo::local, pkgs);
      break;
    }

    case Repo::remote: {
      std::stringstream args;
      args << "rquery -a '%o"
           << delimiter
           << "%v"
           << delimiter
           << "%c"
           << delimiter
           << "%e"
           << delimiter
           << "'";
      pkgs = runPkg(args.str());
      fillPkgRepo(Repo::remote, pkgs);
      break;
    }

    default:
      break;
  }
}

std::tuple<bool, std::string> Pkg::extractToken(FILE * fp, const char delim) const {
  std::string token;

  for (;;) {
    int c = fgetc(fp);

    if (c == EOF) {
      return std::make_tuple(true, std::string());
    } else if (c == delim) {
      return std::make_tuple(false, token);
    } else {
      token.push_back(c);
    }
  }
}

void Pkg::execPkg(const std::string& args) const {
  std::string cmd("pkg " + args);

  FILE * pipe = popen(cmd.c_str(), "r");
  if (!pipe) {
    throw std::runtime_error("Pkg::execPkg(): could not execute [" + cmd + "]");
  }
  pclose(pipe);
}

std::vector<Pkg::Port> Pkg::runPkg(const std::string & args) const {
  std::string cmd("pkg " + args);

  FILE * pipe = popen(cmd.c_str(), "r");
  if (!pipe)
    throw std::runtime_error("Pkg::runPkg(): could not execute [" + cmd + "]");

  std::vector<Pkg::Port> result;

  bool eof;
  do {
    struct Port port;

    std::tie(eof, port.origin) = extractToken(pipe, delimiter);
    if (eof)
      break;

    std::tie(eof, port.remoteVersion) = extractToken(pipe, delimiter);
    if (eof)
      break;

    std::tie(eof, port.comment) = extractToken(pipe, delimiter);
    if (eof)
      throw std::runtime_error("Pkg::runPkg(): EOF reached when reading comment for [" 
                               + port.origin + "]");

    std::tie(eof, port.description) = extractToken(pipe, delimiter);
    if (eof)
      throw std::runtime_error("Pkg::runPkg(): EOF reached when reading descr for [" 
                               + port.origin + "]");

    // discard end of line
    fgetc(pipe);

    result.push_back(port);

  } while (!eof);

  pclose(pipe);

  return result;
}

std::vector<Pkg::Port> Pkg::runPkgSearch(const std::string & args) const {
  std::string cmd("pkg " + args);

  FILE * pipe = popen(cmd.c_str(), "r");
  if (!pipe)
    throw std::runtime_error("Pkg::runPkgSearch(): could not execute [" + cmd + "]");

  std::vector<Pkg::Port> result;

  bool eof;
  char buf[1024];
  do
  {
    struct Port port;

    std::tie(eof, port.origin) = extractToken(pipe, ' ');
    if (eof)
      break;

    // discard to the end of line
    fgets(buf, sizeof(buf), pipe);

    result.push_back(port);
  } while (!eof);

  pclose(pipe);

  return result;
}


std::vector<std::string> Pkg::getPkgOrigins() const {
  std::vector<std::string> origins;

  for (const auto& pkg : *pkgs_)
    for (const auto& port : pkg.second)
      origins.push_back(port.origin);

  return origins;
}

std::vector<std::string> Pkg::getPkgOrigins(const std::string& category) const {
  std::vector<std::string> portsList;

  if (pkgs_->find(category) != pkgs_->end())
    for (const auto& port : pkgs_->at(category))
      portsList.push_back(port.origin);

  return portsList;
}

Pkg::Status Pkg::getStatus(const std::string& origin) const {
  const Port& port = getPort(origin);

  return port.status;
}

std::string Pkg::getLocalVersion(const std::string& origin) const {
  const Port& port = getPort(origin);

  return port.localVersion;
}

std::string Pkg::getRemoteVersion(const std::string& origin) const {
  const Port& port = getPort(origin);

  return port.remoteVersion;
}


std::string Pkg::getPkgAttr(const std::string& origin, Attr attr) const {
  const Port& port = getPort(origin);
  switch (attr) {
    case Attr::origin:
      return port.origin;
    case Attr::status:
      return getCurrentStatusAsString(port.origin);
    case Attr::category:
      return getCategoryFromOrigin(port.origin);
    case Attr::name:
      return getNameFromOrigin(port.origin);
    case Attr::comment:
      return port.comment;
    case Attr::description:
      return port.description;
    case Attr::localVersion:
      return port.localVersion;
    case Attr::remoteVersion:
      return port.remoteVersion;
  }
}

std::vector<std::string> Pkg::getPkgCategories() const {
  std::vector<std::string> categories;
  for (const auto & category : (*pkgs_))
    categories.push_back(category.first);

  return categories;
}

unsigned int Pkg::getCategorySize(const std::string& category) const {
  unsigned int size = 0;

  if (pkgs_->find(category) != pkgs_->end())
    size = pkgs_->at(category).size();

  return size;
}

void Pkg::fillPkgRepo(Repo repo, std::vector<Port>& pkgs) {
  for (auto& port : pkgs) {
    if (repo == Repo::local) {
      port.status.set(Statuses::installed);
      port.localVersion = port.remoteVersion;
    } else {
      port.status.set(Statuses::available);
    }

    std::string portCategory = getCategoryFromOrigin(port.origin);
    std::set<Port>::iterator it = (*pkgs_)[portCategory].find(port);

    // The local repository is filled after the remote was,
    // hence we need to update fields specific to local status
    // and version. Otherwise we can insert the port as is.
    if ((*pkgs_)[portCategory].end() != it) {
      it->status = port.status;
      it->localVersion = port.localVersion;
      // For now let's assume that if the port's local and
      // remote version differ, then the port is outdated.
      // This does not take into account the fact that a
      // port could be created locally based on a version
      // of the software newer than the one available in
      // remote repositories. I expect this case to be an
      // exception to avoid costly comparisons.
      if (it->localVersion != it->remoteVersion) {
        it->status.set(Statuses::upgradable);
      }
    } else {
      (*pkgs_)[portCategory].insert(port);
    }
  }
}

void Pkg::fillTmpRepo(std::vector<Port>& pkgs) {
  tmpPkgs_.clear();
  switchToTemporaryRepository();
  fillPkgRepo(Repo::tmp, pkgs);
}

std::string Pkg::getCurrentStatusAsString(const std::string& origin) const {
  const Pkg::Port& port = getPort(origin);
  return port.status[installed] ? "+" : "-";
}

std::string Pkg::getPendingStatusAsString(const std::string& origin) const {
  const Pkg::Port& port = getPort(origin);
  return port.status[pendingInstall] ? "+" : "-";
}

bool Pkg::hasPendingActions(const std::string& origin) const {
  const Pkg::Port& port = getPort(origin);
  return (port.status[pendingInstall] || port.status[pendingRemoval]);
}

bool Pkg::isUpgradable(const std::string& origin) const {
  const Pkg::Port& port = getPort(origin);
  return (port.status[upgradable]);
}

const Pkg::Port& Pkg::getPort(const std::string& origin) const {
  std::string category = getCategoryFromOrigin(origin);

  if (refPkgs_.find(category) != refPkgs_.end()) {
    const std::set<Port>& portSet = refPkgs_.at(category);
    for (const auto& port : portSet)
      if (port.origin == origin)
        return port;
  }

  throw std::runtime_error("Pkg::getPort(): port [" + origin + "] not found");
}

std::string Pkg::getCategoryFromOrigin(const std::string& origin) const {
  return std::string(origin, 0, origin.find('/'));
}

std::string Pkg::getNameFromOrigin(const std::string& origin) const {
  return std::string(origin, origin.find('/') + 1);
}

void Pkg::registerInstall(const std::string& origin) {
  const Port& port = getPort(origin);
  if (!port.status[installed])
    port.status.set(pendingInstall);
  else {
    if (port.status[pendingRemoval])
      port.status.reset(pendingRemoval);
    else if (port.status[upgradable])
      port.status.set(pendingInstall);
  }
}

void Pkg::registerRemoval(const std::string& origin) {
  const Port& port = getPort(origin);
  if (port.status[pendingInstall])
    port.status.reset(pendingInstall);
  else if (port.status[installed])
    port.status.set(pendingRemoval);
}

void Pkg::performPending() {
  std::string install, remove;

  for (const auto& category : refPkgs_) {
    for (const auto& port : category.second) {
      if (port.status[pendingInstall]) {
        install.append(" ");
        install.append(port.origin);
      } else if (port.status[pendingRemoval]) {
        remove.append(" ");
        remove.append(port.origin);
      }
    }
  }

  if (!remove.empty())
    execPkg("delete -qy" + remove);

  if (!install.empty())
    execPkg("install -qy" + install);

  if (!remove.empty() || !install.empty()) {
    reload();
    resetPending();
  }
}

void Pkg::resetPending() {
  for (const auto& category : refPkgs_)
    for (const auto& port : category.second) {
      port.status.reset(pendingInstall);
      port.status.reset(pendingRemoval);
    }
}

void Pkg::search(const std::string & search) {
  std::string args = "search -o ";
  args.append(search);
  std::vector<Port> pkgs = runPkgSearch(args);
  fillTmpRepo(pkgs);
}

void Pkg::resetFilter() {
  switchToReferenceRepository();
}

void Pkg::applyFilter(const Status& wantedStatuses) {
  std::vector<Port> pkgs;
  for (const auto& category : refPkgs_) {
    for (const auto& pkg : category.second) {
      if ((pkg.status & wantedStatuses).any()) {
        pkgs.push_back(pkg);
      }
    }
  }
  fillTmpRepo(pkgs);
}

}
