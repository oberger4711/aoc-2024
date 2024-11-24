#include <iostream>
#include <fstream>
#include <string>
#include "utils.h"

void solvePart1() {
  std::ifstream ifs("input.txt");
  std::vector<std::string> lines;
  std::string line;
  while (std::getline(ifs, line)) {
    lines.push_back(line);
  }
}

void solvePart2() {
  // TODO
}

int main() {
  measureTime(&solvePart1, "Part 1");
  measureTime(&solvePart2, "Part 2");
  std::cout << "Done.\n";
  return 0;
}