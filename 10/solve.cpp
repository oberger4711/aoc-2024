#include "utils.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <set>

using Coords = Coords_<int>;

struct Data {
  std::vector<std::string> lines;
};

Data parseFile(std::ifstream &ifs) {
  Data data;
  std::string line;
  while (std::getline(ifs, line)) {
    data.lines.push_back(line);
  }
  return data;
}

struct Cell {
  int preds = 0;
  std::set<Coords> heads;
};

using CandidatesMap = std::unordered_map<Coords, Cell, CoordsHash>;

CandidatesMap extractInitialCandidates(const Data &data) {
  CandidatesMap candidates;
  for (int row = 0; row < data.lines.size(); ++row) {
    for (int col = 0; col < data.lines[row].size(); ++col) {
      if (data.lines[row][col] == '0') {
        Cell cell{1};
        cell.heads.insert(Coords(row, col));
        candidates[Coords(row, col)] = cell;
      }
    }
  }
  return candidates;
}

void getConnectedCoords(const Coords &coords, const Coords &bounds,
                        std::vector<Coords> &neighbors) {
  neighbors.clear();
  if (coords.row > 0) {
    neighbors.emplace_back(coords.row - 1, coords.col);
  }
  if (coords.col > 0) {
    neighbors.emplace_back(coords.row, coords.col - 1);
  }
  if (coords.row + 1 < bounds.row) {
    neighbors.emplace_back(coords.row + 1, coords.col);
  }
  if (coords.col + 1 < bounds.col) {
    neighbors.emplace_back(coords.row, coords.col + 1);
  }
}

void solvePart1And2(const Data &data) {
  assert(!data.lines.empty());
  assert(!data.lines[0].empty());
  const Coords bounds(data.lines.size(), data.lines[0].size());
  auto candidates = extractInitialCandidates(data);
  std::vector<Coords> candidateNeighborCoords; // Reused.
  for (int i = 0; i < 9; ++i) {
    int nextNum = i + 1;
    char nextChar = '0' + nextNum;
    CandidatesMap nextCandidates;
    for (const auto &candidatePair : candidates) {
      const Coords coords = candidatePair.first;
      const Cell &cell = candidatePair.second;
      getConnectedCoords(coords, bounds, candidateNeighborCoords);
      for (const auto neighborCoords : candidateNeighborCoords) {
        if (data.lines[neighborCoords.row][neighborCoords.col] == nextChar) {
          nextCandidates[neighborCoords].preds += cell.preds;
          nextCandidates[neighborCoords].heads.insert(cell.heads.begin(), cell.heads.end());
        }
      }
    }
    candidates = std::move(nextCandidates);
  }
  int score = 0;
  int sum = 0;
  for (const auto &sol : candidates) {
    sum += sol.second.preds;
    score += sol.second.heads.size();
  }
  std::cout << score << "\n";
  std::cout << sum << "\n";
}

int main() {
  std::ifstream ifs("input.txt");
  //std::ifstream ifs("input_test.txt");
  //std::ifstream ifs("input_test1.txt");
  const auto data = parseFile(ifs);
  measureTime([&data]() { solvePart1And2(data); }, "Part 1 + 2");
  std::cout << "Done.\n";
  return 0;
}