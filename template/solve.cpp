#include "utils.h"
#include <fstream>
#include <iostream>
#include <string>

std::vector<std::string> parseFile(std::ifstream &ifs) {
  std::vector<std::string> lines;
  std::string line;
  int row = 0;
  while (std::getline(ifs, line)) {
    lines.push_back(line);
  }
  return lines;
}

void solvePart1(const std::vector<std::string> &lines) {
  // TODO
}

void solvePart2(const std::vector<std::string> &lines) {
  // TODO
}

int main() {
  // std::ifstream ifs("input.txt");
  std::ifstream ifs("input_test.txt");
  const auto lines = parseFile(ifs);
  measureTime([&lines]() { solvePart1(lines); }, "Part 1");
  measureTime([&lines]() { solvePart2(lines); }, "Part 2");
  std::cout << "Done.\n";
  return 0;
}