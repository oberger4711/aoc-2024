#include "utils.h"
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <unordered_set>
#include <string>

using Coords = Coords_<int>;

struct Data {
  Coords size;
  std::map<char, std::vector<Coords>> coordsByFreq;
};

void printData(const Data &data) {
  for (const auto &pair : data.coordsByFreq) {
    std::cout << pair.first << ": ";
    for (const auto &coords : pair.second) {
      std::cout << "(" << coords.row << ", " << coords.col << "), ";
    }
    std::cout << "\n";
  }
  std::cout << "size: " << data.size << "\n";
}

Data parseFile(std::ifstream &ifs) {
  Data data;
  std::string line;
  int row = 0;
  while (std::getline(ifs, line)) {
    data.size.col = line.size();
    for (int col = 0; col < line.size(); ++col) {
      const char ch = line[col];
      if (ch != '.') {
        data.coordsByFreq[ch].emplace_back(row, col);
      }
    }
    ++row;
  }
  data.size.row = row;
  // printData(data);
  return data;
}

void solvePart1(const Data &data) {
  const Coords size = data.size;
  std::set<Coords> knownAntiNodes;
  for (const auto &pair : data.coordsByFreq) {
    const auto &coords = pair.second;
    for (int i = 0; i < coords.size(); ++i) {
      for (int j = 0; j < coords.size(); ++j) {
        if (i != j) {
          const auto &coordsI = coords[i];
          const auto &coordsJ = coords[j];
          const Coords diff = coordsJ - coordsI;
          const auto coordsAntiNode = coordsJ + diff;
          if (inBounds(coordsAntiNode, data.size)) {
            knownAntiNodes.insert(coordsAntiNode);
          }
        }
      }
    }
  }
  std::cout << knownAntiNodes.size() << "\n";
}

// Euclidean algorithm
int ggt(int a, int b) {
  if (a == 0) {
    return b;
  } else {
    while (b != 0) {
      if (a > b) {
        a = a - b;
      } else {
        b = b - a;
      }
    }
    return a;
  }
}

Coords reduce(const Coords &coords) {
  return coords / ggt(std::abs(coords.row), std::abs(coords.col));
}

void solvePart2(const Data &data) {
  const Coords size = data.size;
  std::set<Coords> knownAntiNodes;
  for (const auto &pair : data.coordsByFreq) {
    const auto &coords = pair.second;
    for (int i = 0; i < coords.size(); ++i) {
      for (int j = 0; j < coords.size(); ++j) {
        if (i != j) {
          const auto &coordsI = coords[i];
          const auto &coordsJ = coords[j];
          const Coords diff = reduce(coordsJ - coordsI);
          Coords coordsAntiNode = coordsJ;
          int s = 0;
          while (true) {
            const auto coordsAntiNode = coordsJ + (diff * s);
            if (inBounds(coordsAntiNode, data.size)) {
              knownAntiNodes.insert(coordsAntiNode);
              ++s;
            } else {
              break;
            }
          }
        }
      }
    }
  }
  std::cout << knownAntiNodes.size() << "\n";
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