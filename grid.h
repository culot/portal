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

#include <vector>

namespace portal {

template <typename ValueType>
class Grid {
 public:

  struct Coord {
    unsigned long  row;
    unsigned long  col;
  };

  class GridRow {
   public:
    GridRow();
    explicit GridRow(unsigned long width);
    ~GridRow();

    void              clear();
    void              resize(unsigned long width);
    void              set(unsigned long colNum, const ValueType& value);
    ValueType&        getColumn(unsigned long colNum);

   private:
    using RowType = std::vector<ValueType>;

    RowType  row_;
  };

  Grid();
  explicit Grid(unsigned long width);
  ~Grid();

  void              clear();
  unsigned long     height() const {return grid_.size();}
  unsigned long     width() const {return width_;}
  void              resize(unsigned long width);
  bool              inBounds(Coord coord) const;
  void              addRow();
  void              removeRow();
  void              set(unsigned long colNum, const ValueType& value);
  GridRow&          getCurrentRow();
  GridRow&          getRow(unsigned long rowNum);
  ValueType&        getValueAt(Coord coord);

 private:
  using  GridType = std::vector<GridRow>;

  unsigned long  width_ {0};
  unsigned long  currentRow_ {0};

  GridType  grid_;

};

template <typename ValueType>
Grid<ValueType>::Grid() {}

template <typename ValueType>
Grid<ValueType>::Grid(unsigned long width)
    : width_(width),
      grid_(1, GridRow(width)) {}

template <typename ValueType>
Grid<ValueType>::~Grid() {
  grid_.clear();
}

template <typename ValueType>
void Grid<ValueType>::clear() {
  for (auto & row : grid_)
    row.clear();
  grid_.clear();
}

template <typename ValueType>
bool Grid<ValueType>::inBounds(Coord coord) const {
  return coord.row < grid_.size() && coord.col < width_;
}

template <typename ValueType>
void Grid<ValueType>::addRow() {
  GridRow row(width_);
  grid_.push_back(row);
  ++currentRow_;
}

template <typename ValueType>
void Grid<ValueType>::removeRow() {
  grid_.pop_back();
  --currentRow_;
}

template <typename ValueType>
void Grid<ValueType>::set(unsigned long colNum, const ValueType& value) {
  GridRow & row = getCurrentRow();
  row.set(colNum, value);
}

template <typename ValueType>
typename Grid<ValueType>::GridRow& Grid<ValueType>::getCurrentRow() {
  return getRow(currentRow_);
}

template <typename ValueType>
typename Grid<ValueType>::GridRow& Grid<ValueType>::getRow(unsigned long rowNum) {
  if (rowNum >= grid_.size())
    throw std::out_of_range("Grid::getRow(): Row number (" 
                            + std::to_string(rowNum)
                            + ") larger than grid height (" 
                            + std::to_string(grid_.size())
                            + ")");

  return grid_[rowNum];
}

template <typename ValueType>
ValueType& Grid<ValueType>::getValueAt(Coord coord) {
  GridRow & row = getRow(coord.row);

  return row.getColumn(coord.col);
}

template <typename ValueType>
void Grid<ValueType>::resize(unsigned long width) {
  width_ = width;

  for (auto & row : grid_)
    row.resize(width_);
}

template <typename ValueType>
Grid<ValueType>::GridRow::GridRow() : row_() {}

template <typename ValueType>
Grid<ValueType>::GridRow::GridRow(unsigned long width) : row_(width) {}

template <typename ValueType>
Grid<ValueType>::GridRow::~GridRow() {
  row_.clear();
}

template <typename ValueType>
void Grid<ValueType>::GridRow::clear() {
  row_.clear();
}

template <typename ValueType>
void Grid<ValueType>::GridRow::set(unsigned long colNum, const ValueType& value) {
  ValueType& val = getColumn(colNum);
  val = value;
}

template <typename ValueType>
ValueType& Grid<ValueType>::GridRow::getColumn(unsigned long colNum) {
  if (colNum >= row_.size())
    throw std::out_of_range("Grid::getColumn(): Column number (" 
                            + std::to_string(colNum) 
                            + ") larger than grid width (" 
                            + std::to_string(row_.size())
                            + ")");

  return row_[colNum];
}

template <typename ValueType>
void Grid<ValueType>::GridRow::resize(unsigned long width) {
  row_.resize(width);
}

}
