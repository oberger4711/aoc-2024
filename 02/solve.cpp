#include "utils.h"
#include <cassert>
#include <charconv>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>

template <typename T>
std::vector<std::vector<T>> parseCSVNumbers(std::ifstream &ifs,
                                            char delimiter) {
  std::vector<std::vector<T>> ret;
  std::string line;
  while (std::getline(ifs, line)) {
    std::vector<T> nums;
    size_t p1 = 0, p2 = 0;
    do {
      p2 = std::min(line.length(), line.find(delimiter, p1));
      const auto sv = std::string_view(line).substr(p1, p2 - p1);
      T v;
      if (std::from_chars(sv.data(), sv.data() + sv.length(), v).ec ==
          std::errc{}) {
        nums.push_back(v);
      }
      p1 = p2 + 1;
    } while (p1 < line.length());
    ret.emplace_back(std::move(nums));
  }
  return ret;
}

inline bool isValidInc(int diff) { return (1 <= diff && diff <= 3); }

inline bool isValidDec(int diff) { return (-3 <= diff && diff <= -1); }

void solvePart1(const std::vector<std::vector<int>> &nums) {
  size_t safeCount = 0;
  for (const auto row : nums) {
    assert(row.size() > 0);
    bool inc = true, dec = true;
    for (size_t i = 1; i < row.size() && (inc || dec); ++i) {
      const int diff = row[i] - row[i - 1];
      inc = inc && isValidInc(diff);
      dec = dec && isValidDec(diff);
    }
    const bool safe = (inc || dec);
    // std::cout << safe << "\n";
    if (safe) {
      ++safeCount;
    }
  }
  std::cout << safeCount << "\n";
}

template <typename Func>
int check(const std::vector<int> &row, Func func, int skip = -1) {
  int i = 1, prev = 0;
  if (skip == 0) {
    i = 2;
    prev = 1;
  }
  for (; i < row.size(); ++i) {
    if (i != skip) {
      const int diff = row[i] - row[prev];
      if (!func(diff)) {
        return i;
      }
      prev = i;
    }
  }
  return -1;
}

template <typename Func>
bool checkRow(const std::vector<int>& row, Func func) {
  const int skip = check(row, func);
  if (skip == -1) {
    // No skip needed.
    return true;
  }
  else {
    if (check(row, func, skip) == -1) {
      return true;
    }
    else {
      return check(row, func, skip - 1) == -1;
    }
  }
}

void solvePart2(const std::vector<std::vector<int>> &nums) {
  size_t safeCount = 0;
  for (const auto &row : nums) {
    const bool safe = checkRow(row, isValidInc) || checkRow(row, isValidDec);
    //std::cout << safe << ": " << row << "\n";
    if (safe) {
      ++safeCount;
    }
  }
  std::cout << safeCount << "\n";
}

int main() {
  std::ifstream ifs("input.txt");
  //std::ifstream ifs("input_test.txt");
  const auto nums = parseCSVNumbers<int>(ifs, ' ');
  measureTime([&nums]() { solvePart1(nums); }, "Part 1");
  measureTime([&nums]() { solvePart2(nums); }, "Part 2");
  std::cout << "Done.\n";
  return 0;
}