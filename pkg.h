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

#include <string>
#include <tuple>
#include <vector>
#include <set>
#include <map>

namespace portal {

class Pkg {
 public:
  enum class Repo {
    local,
    remote,
    tmp,
    all
  };

  enum class Attr {
    origin,
    status,
    category,
    name,
    comment,
    description,
    nbinfofields
  };

  enum class Status {
    unset,
    uninstalled,
    installed,
    numStatus
  };

  static Pkg&    instance() {static Pkg instance_; return instance_;}

  std::vector<std::string>  getPkgOrigins() const;
  std::vector<std::string>  getPkgOrigins(const std::string& category) const;
  std::string               getNameFromOrigin(const std::string& origin) const;
  Status                    getCurrentStatus(const std::string& origin) const;
  Status                    getPendingStatus(const std::string& origin) const;
  std::vector<std::string>  getPkgCategories() const;
  unsigned int              getCategorySize(const std::string& category) const;
  std::string               getPkgAttr(const std::string& origin, Attr attr) const;
  void                      reload(Repo repo = Repo::all);
  void                      registerPending(const std::string& origin, Pkg::Status status);
  void                      performPending();
  void                      search(const std::string& args);
  void                      filter(Pkg::Status status);
  std::string               getStatusAsString(Pkg::Status status) const;

 private:
  struct Port {
    mutable Status          currentStatus {Status::unset};
    mutable Status          pendingStatus {Status::unset};

    std::string             origin;
    std::string             comment;
    std::string             description;

    bool operator<(const Port & other) const {return origin < other.origin;} 
  };

  Pkg() {}
  Pkg(const Pkg&) = delete;
  void operator=(const Pkg&) = delete;

  // category -> set of Ports
  using PkgRepo = std::map<std::string, std::set<Port>>;

  PkgRepo   refPkgs_; // to store local and remote packages set
  PkgRepo   tmpPkgs_; // to store search/filter result set
  PkgRepo*  pkgs_;    // pointer to the currently used package repository

  void                            buildPackagesList(Repo repo);
  void                            execPkg(const std::string& args) const;
  std::vector<Port>               runPkg(const std::string& args) const;
  std::vector<Port>               runPkgSearch(const std::string& args) const;
  void                            fillPkgRepo(Repo repo, std::vector<Port>& pkgs);
  void                            fillTmpRepo(std::vector<Port>& pkgs);
  const Pkg::Port&                getPort(const std::string& origin) const;
  std::tuple<bool, std::string>   extractToken(FILE* fp, const char delim) const;
  std::string                     getCategoryFromOrigin(const std::string& origin) const;
  void                            resetPending();
  void                            switchToReferenceRepository() {pkgs_ = &refPkgs_;}
  void                            switchToTemporaryRepository() {pkgs_ = &tmpPkgs_;}
};

}
