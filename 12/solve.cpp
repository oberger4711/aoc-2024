#include "utils.h"
#include <algorithm>
#include <array>
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <tuple>

using Coords = Coords_<int>;
using num = long;

struct Data {
  std::vector<std::string> lines;
  int rows;
  int cols;
};

Data parseFile(std::ifstream &ifs) {
  Data data;
  std::string line;
  while (std::getline(ifs, line)) {
    data.lines.push_back(line);
  }
  data.rows = data.lines.size();
  assert(data.rows > 0);
  data.cols = data.lines[0].size();
  return data;
}

struct Cell {
  Cell() = default;
  Cell(char ch_) : ch(ch_) {}

  char ch;
  num id = -1; // -1 if not assigned yet.
};

struct Garden {
  std::vector<std::vector<Cell>> cells;
  Coords size;

  Garden(const Data &data) : size(data.rows, data.cols) {
    cells.reserve(data.rows);
    for (int r = 0; r < data.rows; ++r) {
      auto &line = data.lines[r];
      cells.emplace_back();
      std::transform(line.begin(), line.end(), std::back_inserter(cells.back()),
                     [](char ch) { return Cell(ch); });
    }
  }

  Cell &operator()(int row, int col) { return cells[row][col]; }
  Cell &operator()(const Coords &coords) {
    return cells[coords.row][coords.col];
  }
};

struct CoordsToExplore {
  CoordsToExplore() = default;
  CoordsToExplore(const Coords &coords_, Cell *cell_, char predDirIndex_)
      : coords(coords_), cell(cell_), predDirIndex(predDirIndex_) {}

  Coords coords;
  Cell *cell;
  char predDirIndex;

  static constexpr char NO_PRED_DIR_INDEX = 5;
};

num explore1(Garden &garden, int id, int row, int col) {
  num perimeterSum = 0;
  num areaSum = 0;
  Cell &seedCell = garden(row, col);
  if (seedCell.id == -1) {
    static const std::array<Coords, 4> dirs = {Coords::Left(), Coords::Right(),
                                               Coords::Up(), Coords::Down()};
    // This maps an index to a direction to its inverse direction:
    static const std::array<char, 4> invDirIndices = {1, 0, 3, 2};
    const char seedCh = garden(row, col).ch;
    // Depth first currently because maybe vector is faster than queue here (is
    // it?).
    std::vector<CoordsToExplore> q;
    q.emplace_back(Coords(row, col), &seedCell,
                   CoordsToExplore::NO_PRED_DIR_INDEX);
    while (!q.empty()) {
      const auto coordsToExplore = q.back();
      q.pop_back();
      // Cell lookup already happened at predecessor's expansion.
      Cell &cell = *coordsToExplore.cell;
      // It might be that the cell was explored since adding it to q.
      if (cell.id != id) {
        cell.id = id;
        int perimeter = 4;
        for (int iDir = 0; iDir < 4; ++iDir) {
          // Skip predecessor.
          if (iDir != coordsToExplore.predDirIndex) {
            const auto &dir = dirs[iDir];
            const Coords neighCoords = coordsToExplore.coords + dir;
            if (inBounds(neighCoords, garden.size)) {
              Cell &neighCell = garden(neighCoords);
              if (neighCell.ch == seedCh) {
                --perimeter;
                // Do not expand already explored cells again.
                if (neighCell.id != id) {
                  q.emplace_back(neighCoords, &neighCell, invDirIndices[iDir]);
                }
              }
            }
          } else {
            --perimeter;
          }
        }
        ++areaSum;
        perimeterSum += perimeter;
      }
    }
  }
  return areaSum * perimeterSum;
}

void solvePart1(const Data &data_) {
  Garden garden(data_);
  num sum = 0;
  num id = 0;
  for (int row = 0; row < garden.size.row; ++row) {
    for (int col = 0; col < garden.size.col; ++col) {
      const num price = explore1(garden, id, row, col);
      sum += price;
      if (price > 0) {
        ++id;
      }
    }
  }
  std::cout << sum << "\n";
}

auto getRow(const Coords &c) { return c.row; }

auto getCol(const Coords &c) { return c.col; }

template <typename GetMajorComp, typename GetMinorComp>
num countConnectedSides(std::vector<Coords> &sides, GetMajorComp getMajorComp,
                        GetMinorComp getMinorComp) {
  // The ordering depends on the side direction.
  auto compare = [getMajorComp, getMinorComp](const Coords &lhs,
                                              const Coords &rhs) {
    const auto lhsMajor = getMajorComp(lhs);
    const auto rhsMajor = getMajorComp(rhs);
    if (lhsMajor == rhsMajor) {
      return getMinorComp(lhs) < getMinorComp(rhs);
    }
    return lhsMajor < rhsMajor;
  };
  std::sort(sides.begin(), sides.end(), compare);
  num count = 0;
  if (!sides.empty()) {
    count = 1;
    num prevMajorPos = getMajorComp(sides[0]);
    num prevMinorPos = getMinorComp(sides[0]);
    for (const auto &s : sides) {
      const num majorPos = getMajorComp(s);
      const num minorPos = getMinorComp(s);
      if (majorPos != prevMajorPos || (minorPos - prevMinorPos) > 1) {
        // Discontinuity found.
        ++count;
      }
      prevMajorPos = majorPos;
      prevMinorPos = minorPos;
    }
  }
  return count;
}

num explore2(Garden &garden, int id, int row, int col) {
  num perimeterSum = 0;
  num areaSum = 0;
  Cell &seedCell = garden(row, col);
  std::array<std::vector<Coords>, 4> sidesByDirIndex; // l, r, u, d
  if (seedCell.id == -1) {
    static const std::array<Coords, 4> dirs = {Coords::Left(), Coords::Right(),
                                               Coords::Up(), Coords::Down()};
    // This maps an index to a direction to its inverse direction:
    static const std::array<char, 4> invDirIndices = {1, 0, 3, 2};
    const char seedCh = garden(row, col).ch;
    // Depth first currently because maybe vector is faster than queue here (is
    // it?).
    std::vector<CoordsToExplore> q;
    q.emplace_back(Coords(row, col), &seedCell,
                   CoordsToExplore::NO_PRED_DIR_INDEX);
    while (!q.empty()) {
      const auto coordsToExplore = q.back();
      q.pop_back();
      // Cell lookup already happened at predecessor's expansion.
      Cell &cell = *coordsToExplore.cell;
      // It might be that the cell was explored since adding it to q.
      if (cell.id != id) {
        cell.id = id;
        for (int iDir = 0; iDir < 4; ++iDir) {
          // Skip predecessor.
          if (iDir != coordsToExplore.predDirIndex) {
            const auto &dir = dirs[iDir];
            const Coords neighCoords = coordsToExplore.coords + dir;
            if (inBounds(neighCoords, garden.size)) {
              Cell &neighCell = garden(neighCoords);
              if (neighCell.ch == seedCh) {
                // Do not expand already explored cells again.
                if (neighCell.id != id) {
                  q.emplace_back(neighCoords, &neighCell, invDirIndices[iDir]);
                }
              } else {
                sidesByDirIndex[iDir].push_back(neighCoords);
              }
            } else {
              sidesByDirIndex[iDir].push_back(neighCoords);
            }
          }
        }
        ++areaSum;
      }
    }
    perimeterSum += countConnectedSides(sidesByDirIndex[0], &getCol, &getRow);
    perimeterSum += countConnectedSides(sidesByDirIndex[1], &getCol, &getRow);
    perimeterSum += countConnectedSides(sidesByDirIndex[2], &getRow, &getCol);
    perimeterSum += countConnectedSides(sidesByDirIndex[3], &getRow, &getCol);
  }
  return areaSum * perimeterSum;
}

void solvePart2(const Data &data_) {
  Garden garden(data_);
  num sum = 0;
  num id = 0;
  for (int row = 0; row < garden.size.row; ++row) {
    for (int col = 0; col < garden.size.col; ++col) {
      const num price = explore2(garden, id, row, col);
      sum += price;
      if (price > 0) {
        ++id;
      }
    }
  }
  std::cout << sum << "\n";
}

int main() {
  std::ifstream ifs("input.txt");
  // std::ifstream ifs("input_test.txt");
  const auto data = parseFile(ifs);
  measureTime([&data]() { solvePart1(data); }, "Part 1");
  measureTime([&data]() { solvePart2(data); }, "Part 2");
  std::cout << "Done.\n";
  return 0;
}