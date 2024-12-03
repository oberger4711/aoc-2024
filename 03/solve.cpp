#include "utils.h"
#include <fstream>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <string_view>

std::string readFile(std::ifstream &ifs) {
  std::stringstream sstream;
  sstream << ifs.rdbuf();
  return std::string(sstream.str());
}

void solvePart1(const std::string &text) {
  const char *haystack = text.c_str();
  std::regex mulRegex("mul\\((\\d+),(\\d+)\\)");
  int sum = 0;
  for (std::cmatch cm; regex_search(haystack, cm, mulRegex);) {
    const int a = std::stoi(cm[1].str());
    const int b = std::stoi(cm[2].str());
    sum += a * b;
    haystack = &haystack[cm.position() + cm.length()];
  }
  std::cout << sum << "\n";
}

struct Condition {
  int pos;
  bool enable;
};

std::vector<Condition> findConditions(const std::string &text,
                                      const std::string &regexString,
                                      bool enable) {
  const char *haystack = text.c_str();
  std::vector<Condition> conditions;
  std::regex condRegex(regexString);
  int offset = 0;
  for (std::cmatch cm; regex_search(haystack, cm, condRegex);) {
    const int matchPos = offset + static_cast<int>(cm.position());
    conditions.push_back({matchPos, enable});
    offset = matchPos + cm.length();
    haystack = &haystack[cm.position() + cm.length()];
  }
  return conditions;
}

std::vector<Condition> mergeConditions(const std::vector<Condition> &dos,
                                       const std::vector<Condition> &donts) {
  std::vector<Condition> conditions;
  conditions.reserve(dos.size() + donts.size());
  std::size_t doPos = 0, dontPos = 0;
  while (doPos < dos.size() && dontPos < donts.size()) {
    if (dos[doPos].pos < donts[dontPos].pos) {
      conditions.push_back(Condition{dos[doPos].pos, true});
      ++doPos;
    } else {
      conditions.push_back(Condition{donts[dontPos].pos, false});
      ++dontPos;
    }
  }
  while (doPos < dos.size()) {
    conditions.push_back(Condition{dos[doPos].pos, true});
    ++doPos;
  }
  while (dontPos < donts.size()) {
    conditions.push_back(Condition{donts[dontPos].pos, false});
    ++dontPos;
  }
  return conditions;
}

void solvePart2(const std::string &text) {
  const auto dos = findConditions(text, "do\\(\\)", true);
  const auto donts = findConditions(text, "don\\'t\\(\\)", false);
  auto conditions = mergeConditions(dos, donts);
  std::reverse(conditions.begin(), conditions.end());
  const char *haystack = text.c_str();
  std::regex mulRegex("mul\\((\\d+),(\\d+)\\)");
  int sum = 0;
  int lastMatchPos = 0;
  bool enabled = true;
  int offset = 0;
  for (std::cmatch cm; regex_search(haystack, cm, mulRegex);) {
    const int matchPos = offset + static_cast<int>(cm.position());
    while (!conditions.empty() && conditions.back().pos < matchPos) {
      enabled = conditions.back().enable;
      conditions.pop_back();
    }
    if (enabled) {
      const int a = std::stoi(cm[1].str());
      const int b = std::stoi(cm[2].str());
      sum += a * b;
    }
    offset = matchPos + cm.length();
    haystack = &haystack[cm.position() + cm.length()];
  }
  std::cout << sum << "\n";
}

int main() {
  std::ifstream ifs("input.txt");
  // std::ifstream ifs("input_test.txt");
  std::string text = readFile(ifs);
  measureTime([&text]() { solvePart1(text); }, "Part 1");
  measureTime([&text]() { solvePart2(text); }, "Part 2");
  std::cout << "Done.\n";
  return 0;
}