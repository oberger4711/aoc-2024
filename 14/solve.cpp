#include "utils.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

using num = int;
using Coords = Coords_<num>;

// Modulo returning positive number.
inline int mod(int i, int n) { return (i % n + n) % n; }

Coords extractNextCoords(const std::string &line, std::size_t *start) {
  auto s = std::min(line.find('p', *start), line.find('v', *start)) + 1;
  while (line[s] == '=' || line[s] == '+') {
    ++s;
  }
  auto e =
      std::min(line.size(), std::min(line.find(',', s), line.find(' ', s)));
  const auto c1 = line.substr(s, e - s);
  s = e + 1;
  e = std::min(line.size(), std::min(line.find(',', s), line.find(' ')));
  const auto c2 = line.substr(s, e - s);
  *start = e;
  return Coords(std::stoi(c2), std::stoi(c1));
}

struct Robot {
  Robot() = default;
  Robot(const Coords &pos_, const Coords &vel_) : pos(pos_), vel(vel_) {}
  Coords pos;
  Coords vel;
};

struct Data {
  std::vector<Robot> robots;
  Coords roomSize;
};

Data parseFile(std::ifstream &ifs) {
  Data data;
  std::string line;
  while (std::getline(ifs, line)) {
    size_t i = 0;
    const Coords startPos = extractNextCoords(line, &i);
    const Coords vel = extractNextCoords(line, &i);
    data.robots.emplace_back(startPos, vel);
  }
  return data;
}

// Returns -1 if on the edge.
int evalQuadrant(const Coords &pos, const Coords &roomSize) {
  int quadrant = -1;
  const Coords edge = roomSize / 2;
  if (pos.row != edge.row && pos.col != edge.col) {
    const bool top = pos.row < edge.row;
    const bool left = pos.col < edge.col;
    if (top && !left) {
      quadrant = 0;
    } else if (top && left) {
      quadrant = 1;
    } else if (!top && left) {
      quadrant = 2;
    } else if (!top && !left) {
      quadrant = 3;
    }
  }
  return quadrant;
}

void solvePart1(const Data &data) {
  std::array<num, 4> numRobotsInQuadrants = {0, 0, 0, 0};
  constexpr num iterations = 100;
  for (const auto &def : data.robots) {
    Coords pos = def.pos + (def.vel * iterations);
    pos.row = mod(pos.row, data.roomSize.row);
    pos.col = mod(pos.col, data.roomSize.col);
    // std::cout << def.startPos << " -> " << pos << "\n";
    const auto q = evalQuadrant(pos, data.roomSize);
    if (q >= 0) {
      ++numRobotsInQuadrants[q];
    }
  }
  // std::cout << "Quadrants: " << numRobotsInQuadrants[0] << ", "
  //           << numRobotsInQuadrants[1] << ", " << numRobotsInQuadrants[2]
  //           << ", " << numRobotsInQuadrants[3] << "\n";
  num safetyFactor = 1;
  for (auto n : numRobotsInQuadrants) {
    safetyFactor *= n;
  }
  std::cout << safetyFactor << "\n";
}

std::vector<std::string> render(const std::vector<Robot> &robots,
                                const Coords &roomSize) {
  std::vector<std::string> grid;
  std::stringstream ss;
  for (int i = 0; i < roomSize.col; ++i) {
    ss << ".";
  }
  grid.resize(roomSize.row, ss.str());
  for (const auto &robot : robots) {
    grid[robot.pos.row][robot.pos.col] = 'X';
  }
  return grid;
}

void simulate(std::vector<Robot> &robots, const Coords &roomSize) {

  for (auto &def : robots) {
    def.pos = def.pos + def.vel;
    def.pos.row = mod(def.pos.row, roomSize.row);
    def.pos.col = mod(def.pos.col, roomSize.col);
  }
}

void solvePart2(const Data &data) {
  std::array<num, 4> numRobotsInQuadrants = {0, 0, 0, 0};
  constexpr num iterations = 100;
  std::vector<Robot> robots = data.robots;
  // I saw a pattern in the output of iteration 27, 128 and 229.
  // Therefore I decided to print only every 101th iterations starting at 27.
  for (num i = 0; i < 27; ++i) {
    simulate(robots, data.roomSize);
  }
  for (num i = 27; i < 8006;) {
    for (num j = 0; j < 101; ++j, ++i) {
      simulate(robots, data.roomSize);
    }
    // const auto grid = render(robots, data.roomSize);
    // std::cout << grid;
    // std::cout << "Time: " << i << " s\n";
  }
  // Found the tree at iteration 8006 by scrolling through the output of the previous loops.
  // Render iteration with tree.
  const auto grid = render(robots, data.roomSize);
  std::cout << grid;
  std::cout << "8006\n";
}

int main() {
  std::ifstream ifs("input.txt");
  const Coords roomSize(103, 101);

  // std::ifstream ifs("input_test.txt");
  // const Coords roomSize(7, 11);

  auto data = parseFile(ifs);
  data.roomSize = roomSize;
  measureTime([&data]() { solvePart1(data); }, "Part 1");
  measureTime([&data]() { solvePart2(data); }, "Part 2");
  std::cout << "Done.\n";
  return 0;
}