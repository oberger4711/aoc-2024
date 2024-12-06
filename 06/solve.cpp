#include "utils.h"
#include <array>
#include <cassert>
#include <fstream>
#include <iostream>
#include <set>
#include <string>

using Coords = Coords_<int>;

// For std::set.
inline bool operator<(const Coords &lhs, const Coords &rhs) {
  if (lhs.row < rhs.row) {
    return true;
  } else if (lhs.row == rhs.row) {
    return lhs.col < rhs.col;
  } else {
    return false;
  }
}

std::vector<std::string> parseFile(std::ifstream &ifs) {
  std::vector<std::string> lines;
  std::string line;
  int row = 0;
  while (std::getline(ifs, line)) {
    lines.push_back(line);
  }
  return lines;
}

Coords searchStart(const std::vector<std::string> &lines) {
  for (int row = 0; row < lines.size(); ++row) {
    const auto &line = lines[row];
    for (int col = 0; col < line.size(); ++col) {
      if (line[col] == '^') {
        return Coords(row, col);
      }
    }
  }
  assert(false);
  return Coords(0, 0);
}

bool withinBounds(const Coords &pos, const Coords &bounds) {
  return pos.row >= 0 && pos.row < bounds.row && pos.col >= 0 &&
         pos.col < bounds.col;
}

std::set<Coords> solvePart1(const std::vector<std::string> &lines_) {
  assert(lines_.size() > 0);
  assert(lines_[0].size() > 0);
  std::set<Coords> positions;
  std::vector<std::string> lines = lines_; // Copy
  const std::array<Coords, 4> dirs = {Coords(-1, 0), Coords(0, 1), Coords(1, 0),
                                      Coords(0, -1)};
  const Coords bounds(lines.size(), lines[0].size());
  const Coords posStart = searchStart(lines);
  Coords pos = posStart;
  int dirIndex = 0; // Up
  lines[pos.row][pos.col] = 'X';
  int count = 1;
  while (true) {
    const Coords nextPos = pos + dirs[dirIndex];
    if (!withinBounds(nextPos, bounds)) {
      break;
    } else {
      char &ch = lines[nextPos.row][nextPos.col];
      if (ch == '#') {
        dirIndex = (dirIndex + 1) % dirs.size();
      } else {
        // Free.
        if (ch != 'X') {
          // Not seen yet.
          ch = 'X';
          ++count;
        }
        pos = nextPos;
        if (!(pos == posStart)) {
          positions.insert(pos);
        }
      }
    }
  }
  // std::cout << lines << "\n";
  std::cout << count << "\n";
  return positions;
}

// PART 2

constexpr char DIR_CHAR_0 = 'A';

void addDir(char &ch, int dirIndex) {
  char encoding = (1 << dirIndex);
  if (ch == '.') {
    ch = DIR_CHAR_0 + encoding;
  } else {
    ch = DIR_CHAR_0 + ((ch - DIR_CHAR_0) | encoding);
  }
}

bool hasDir(char ch, int dirIndex) {
  if (ch == '.') {
    return false;
  }
  char encoding = (1 << dirIndex);
  return ((ch - DIR_CHAR_0) & encoding) != 0;
}

void solvePart2(const std::vector<std::string> &lines_,
                const std::set<Coords> &obsPositions) {
  assert(lines_.size() > 0);
  assert(lines_[0].size() > 0);
  int count = 0;
  for (const auto &obsPos : obsPositions) {
    std::vector<std::string> lines = lines_; // Copy
    lines[obsPos.row][obsPos.col] = '#';
    const std::array<Coords, 4> dirs = {Coords(-1, 0), Coords(0, 1),
                                        Coords(1, 0), Coords(0, -1)};
    const Coords bounds(lines.size(), lines[0].size());
    Coords pos = searchStart(lines);
    int dirIndex = 0;              // Up
    lines[pos.row][pos.col] = '.'; // Clear '^'.
    addDir(lines[pos.row][pos.col], dirIndex);
    while (true) {
      const Coords nextPos = pos + dirs[dirIndex];
      if (!withinBounds(nextPos, bounds)) {
        break;
      } else {
        char &ch = lines[nextPos.row][nextPos.col];
        if (ch == '#') {
          dirIndex = (dirIndex + 1) % dirs.size();
        } else {
          // Free.
          if (hasDir(ch, dirIndex)) {
            // Loop!
            ++count;
            break;
          } else {
            // No loop
            addDir(ch, dirIndex);
          }
          pos = nextPos;
        }
      }
    }
  }
  std::cout << count << "\n";
}

int main() {
  std::ifstream ifs("input.txt");
  // std::ifstream ifs("input_test.txt");
  const auto lines = parseFile(ifs);
  std::set<Coords> positions;
  measureTime([&lines, &positions]() { positions = solvePart1(lines); },
              "Part 1");
  measureTime([&lines, &positions]() { solvePart2(lines, positions); },
              "Part 2");
  std::cout << "Done.\n";
  return 0;
}