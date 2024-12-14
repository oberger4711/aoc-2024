// NOTE:
// This solution uses libEigen so you will need to have it installed in order to
// compile.

#include "utils.h"
#include <array>
#include <cassert>
#include <eigen3/Eigen/Dense>
#include <fstream>
#include <iostream>
#include <optional>
#include <queue>
#include <string>

using num = long long;
using Coords = Coords_<num>;
using Vec2n = Eigen::Vector<num, 2>;
using Vec2d = Eigen::Vector2d;
using Mat2d = Eigen::Matrix2d;

struct Equation {
  // p = (a b) x;
  // x = (s t)^T
  // Where s = number of presses on button A and t = number of presses on
  // button B.
  Vec2n v1; // First button
  Vec2n v2; // Second button
  Vec2n p;  // Prize position
};

struct Data {
  std::vector<Equation> equations;
};

std::string extractNextComponent(const std::string &line, std::size_t *start) {
  auto s = std::min(line.find('X', *start), line.find('Y', *start)) + 1;
  // For prize line:
  while (line[s] == '=' || line[s] == '+') {
    ++s;
  }
  auto e = std::min(line.size(), line.find(',', s));
  *start = e;
  return line.substr(s, e - s);
}

Vec2n parseCoords(const std::string &line) {
  size_t pos = 7;
  const std::string xStr = extractNextComponent(line, &pos);
  const std::string yStr = extractNextComponent(line, &pos);
  return Vec2n(std::stoi(xStr), std::stoi(yStr));
}

Data parseFile(std::ifstream &ifs) {
  Data data;
  std::string line;
  while (std::getline(ifs, line)) {
    if (!line.empty()) {
      Equation eq;
      eq.v1 = parseCoords(line);
      assert(std::getline(ifs, line));
      eq.v2 = parseCoords(line);
      assert(std::getline(ifs, line));
      eq.p = parseCoords(line);
      data.equations.push_back(std::move(eq));
    }
  }
  return data;
}

bool asInteger(const Vec2d &x, Vec2n *xInt) {
  // Tuned this a bit for part 2 to get the correct solution:
  static constexpr double EPSILON = 1e-3;
  *xInt << std::round(x[0]), std::round(x[1]);
  return std::abs((*xInt)[0] - x[0]) < EPSILON &&
         std::abs((*xInt)[1] - x[1]) < EPSILON;
}

void solve(const Data &data, long offset) {
  num tokens = 0;
  Vec2n costs(3, 1);
  for (const auto &eq : data.equations) {
    // Transform problem to Ax = b and solve for x with libEigen.
    // A = (v1 v2)
    // b = p
    // We need to transform the integers to floating point numbers.
    // This means we have to deal with a bit of conversion issues.
    Mat2d a;
    // clang-format off
    a.col(0) = eq.v1.cast<double>();
    a.col(1) = eq.v2.cast<double>();
    // clang-format on
    const Vec2d b = eq.p.cast<double>() + Vec2d(offset, offset);
    auto lu = a.fullPivLu();
    const Vec2d x = lu.solve(b);
    if (lu.rank() != 2) {
      std::cout << "Not full rank!\n";
    } else {
      Vec2n xInt;
      if (asInteger(x, &xInt)) {
        tokens += xInt.dot(costs);
      }
    }
  }
  std::cout << tokens << "\n";
}

void solvePart1(const Data &data) { solve(data, 0); }
void solvePart2(const Data &data) { solve(data, 10000000000000); }

int main() {
  std::ifstream ifs("input.txt");
  // std::ifstream ifs("input_test.txt");
  const auto data = parseFile(ifs);
  measureTime([&data]() { solvePart1(data); }, "Part 1");
  measureTime([&data]() { solvePart2(data); }, "Part 2");
  std::cout << "Done.\n";
  return 0;
}