#include "utils.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>

struct Data {
  std::vector<std::string> rows;
  std::vector<std::string> cols;
  std::vector<std::string> diags;
  std::vector<std::string> diagsT;
};

std::vector<std::string> transpose(std::vector<std::string> &vec) {
  std::vector<std::string> t;
  for (size_t i = 0; i < vec.size(); ++i) {
    const auto &si = vec[i];
    const auto n = si.size();
    if (t.size() < n) {
      t.resize(n);
    }
    for (size_t j = 0; j < n; ++j) {
      t[j].push_back(si[j]);
    }
  }
  return t;
}

Data parseFile(std::ifstream &ifs) {
  Data data;
  std::string line;
  int i = 0;
  if (std::getline(ifs, line)) {
    const int n = line.size();
    assert(n % 2 == 0);
    const int numDiags = 2 * n - 1;
    data.diags.resize(numDiags);
    data.diagsT.resize(numDiags);
    do {
      data.rows.push_back(line);
      for (size_t j = 0; j < n; ++j) {
        {
          const int d = (n - 1) - i + j;
          assert(d >= 0);
          assert(d < numDiags);
          data.diags[d].push_back(line[j]);
        }
        {
          const int d = i + j;
          assert(d >= 0);
          assert(d < numDiags);
          data.diagsT[d].push_back(line[j]);
        }
      }
      ++i;
    } while (std::getline(ifs, line));
  }
  data.cols = transpose(data.rows);
  // std::cout << data.rows << "\n";
  // std::cout << data.cols << "\n";
  // std::cout << data.diags << "\n";
  // std::cout << data.diagsT << "\n";
  return data;
}

int countXMAS(const std::string &str, const std::string& needle) {
  int count = 0;
  auto s = str.find(needle);
  for (; s != std::string::npos; s = str.find(needle, s + needle.size())) {
    ++count;
  }
  return count;
}

void solvePart1(const Data &data) {
  int count = 0;
  for (const auto channel :
       {&data.rows, &data.cols, &data.diags, &data.diagsT}) {
    for (const auto &str : *channel) {
      count += countXMAS(str, "XMAS");
      count += countXMAS(str, "SAMX");
    }
  }
  std::cout << count << "\n";
}

bool isMAS(char c1, char c2) {
  return (c1 == 'M' && c2 == 'S') || (c1 == 'S' && c2 == 'M');
}

void solvePart2(const Data &data) {
  const auto &rows = data.rows;
  int count = 0;
  const int n = rows.size();
  for (int i = 1; i + 1 < n; ++i) {
    const auto &row = rows[i];
    for (int j = 1; j + 1 < n; ++j) {
      if (row[j] == 'A' && isMAS(rows[i - 1][j - 1], rows[i + 1][j + 1]) &&
          isMAS(rows[i - 1][j + 1], rows[i + 1][j - 1])) {
        ++count;
      }
    }
  }
  std::cout << count << "\n";
}

int main() {
  std::ifstream ifs("input.txt");
  // std::ifstream ifs("input_test.txt");
  //  std::ifstream ifs("input_test1.txt");
  const auto data = parseFile(ifs);
  measureTime([&data]() { solvePart1(data); }, "Part 1");
  measureTime([&data]() { solvePart2(data); }, "Part 2");
  std::cout << "Done.\n";
  return 0;
}