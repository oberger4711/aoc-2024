#include <fstream>
#include <iostream>
#include <string>
#include <array>
#include <algorithm>
#include <unordered_map>

#include "utils.h"

constexpr int LINES = 1000;
using Nums = std::pair<std::array<int, LINES>, std::array<int, LINES>>;

Nums parseNums() {
  std::ifstream ifs("input.txt");
  Nums ret;
  std::string line;
  for (int i = 0; std::getline(ifs, line) && i < LINES; ++i) {
    ret.first[i] = std::stoi(line.substr(0, 5));
    ret.second[i] = std::stoi(line.substr(8, 5));
  }
  return ret;
}

void solvePart1(const Nums& nums) {
  Nums sorted_nums = nums;
  std::sort(sorted_nums.first.begin(), sorted_nums.first.end());
  std::sort(sorted_nums.second.begin(), sorted_nums.second.end());
  int sum = 0;
  for (int i = 0; i < LINES; ++i)
  {
    sum += std::abs(sorted_nums.first[i] - sorted_nums.second[i]);
  }
  std::cout << sum << "\n";
}

void solvePart2(const Nums& nums) {
  std::unordered_map<int, int> occurrences;
  for (auto n : nums.second) {
    ++occurrences[n];
  }
  int sum = 0;
  for (auto n : nums.first) {
    sum += n * occurrences[n];
  }
  std::cout << sum << "\n";
}

int main() {
  const auto nums = parseNums();
  measureTime([&nums]() { solvePart1(nums); }, "Part 1");
  measureTime([&nums]() { solvePart2(nums); }, "Part 2");
  std::cout << "Done.\n";
  return 0;
}