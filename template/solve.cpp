#include "utils.h"
#include <fstream>
#include <iostream>
#include <string>

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

void solvePart1(const Data &data) {
  // TODO
}

void solvePart2(const Data &data) {
  // TODO
}

int main() {
  // std::ifstream ifs("input.txt");
  std::ifstream ifs("input_test.txt");
  const auto data = parseFile(ifs);
  measureTime([&data]() { solvePart1(data); }, "Part 1");
  measureTime([&data]() { solvePart2(data); }, "Part 2");
  std::cout << "Done.\n";
  return 0;
}