#include "utils.h"
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>

using num = long long;

struct Data {
  num result;
  std::vector<num> operands;
};

std::vector<Data> parseFile(std::ifstream &ifs) {
  std::vector<Data> data;
  std::string line;
  num row = 0;
  while (std::getline(ifs, line)) {
    const auto posSeparator = line.find(':');
    Data parsed;
    parsed.result = std::stoll(line.substr(0, posSeparator));
    parsed.operands = parseCSVLine<num>(line.substr(posSeparator + 2), ' ');
    assert(!parsed.operands.empty());
    data.emplace_back(std::move(parsed));
  }
  return data;
}

bool solve(const Data &line, num result, int opIndex);

template <typename Func>
bool solveWithResult(const Data &line, num computedResult, int opIndex,
                     Func solveImpl) {
  if (computedResult > line.result) {
    return false;
  }
  if (opIndex + 1 >= line.operands.size()) {
    return computedResult == line.result;
  }
  return solveImpl(line, computedResult, opIndex + 1);
}

bool solveImpl1(const Data &line, num result, int opIndex) {
  const num operand = line.operands[opIndex];
  const num resultProduct = result * operand;
  const num resultSum = result + operand;
  return solveWithResult(line, resultProduct, opIndex, solveImpl1) ||
         solveWithResult(line, resultSum, opIndex, solveImpl1);
}

num concat(num result, num operand) {
  const num log = static_cast<num>(std::log10(operand));
  num p = std::pow(10, log);
  while (p <= operand) {
    p *= 10;
  }
  // clang-format off
  //std::cout << result << " || " << operand << " = " << p * result + operand << " (log = " << log << ", p = " << p << "\n";
  //  clang-format on
  return p * result + operand;
}

bool solveImpl2(const Data &line, num result, int opIndex) {
  const num operand = line.operands[opIndex];
  const num resultProduct = result * operand;
  const num resultSum = result + operand;
  const num log = std::log10(result);
  const num resultConcat = concat(result, operand);
  return solveWithResult(line, resultProduct, opIndex, solveImpl2) ||
         solveWithResult(line, resultSum, opIndex, solveImpl2) ||
         solveWithResult(line, resultConcat, opIndex, solveImpl2);
}

template <typename Func> bool solve(const Data &line, Func solveImpl) {
  assert(line.operands.size() >= 1);
  const num result = line.operands[0];
  if (line.operands.size() > 1) {
    return solveImpl(line, result, 1);
  } else {
    return result == line.result;
  }
}

void solvePart1(const std::vector<Data> &data) {
  num sum = 0;
  for (Data line : data) {
    if (solve(line, solveImpl1)) {
      sum += line.result;
    }
  }
  std::cout << sum << "\n";
}

void solvePart2(const std::vector<Data> &data) {
  num sum = 0;
  for (Data line : data) {
    if (solve(line, solveImpl2)) {
      num before = sum;
      sum += line.result;
      assert(sum > before);
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