#include "utils.h"
#include <fstream>
#include <iostream>
#include <string>
#include <map>

struct Data {
  std::vector<std::pair<int, int>> rules;
  std::vector<std::vector<int>> updates;
};

Data parseFile(std::ifstream &ifs) {
  Data data;
  std::string line;
  int row = 0;
  while (std::getline(ifs, line) && line != "") {
    data.rules.emplace_back(std::stoi(line.substr(0, 2)),
                            std::stoi(line.substr(3, 2)));
  }
  data.updates = parseCSVNumbers<int>(ifs, ',');
  return data;
}

bool findInMap(const std::map<int, size_t>& numPositions, int num, size_t* outPos) {
  const auto it = numPositions.find(num);
  bool found = (it != numPositions.end());
  if (found) {
    *outPos = it->second;
  }
  return found;
}

void solvePart1And2(Data &data) {
  int sumCorrect = 0;
  int sumIncorrect = 0;
  for (auto& u : data.updates) {
    std::map<int, size_t> numPositions;
    for (size_t i = 0; i < u.size(); ++i) {
      numPositions[u[i]] = i;
    }
    bool initiallyCorrect = true;
    bool correct = false; // For correction of incorrect.
    // Not the fastest solution but it is short.
    while (!correct) {
      correct = true;
      for (const auto& r : data.rules) {
        const int num1 = r.first;
        const int num2 = r.second;
        size_t pos1, pos2;
        if (findInMap(numPositions, num1, &pos1) && findInMap(numPositions, num2, &pos2)) {
          // Both nums of rule are in update.
          if (pos1 > pos2) {
            initiallyCorrect = false;
            correct = false;
            std::swap(u[pos1], u[pos2]);
            std::swap(numPositions[num1], numPositions[num2]);
          }
        }
      }
    }
    if (initiallyCorrect) {
      sumCorrect += u[u.size() / 2];
    }
    else {
      sumIncorrect += u[u.size() / 2];
    }
  }
  std::cout << "Part 1: " << sumCorrect << "\n";
  std::cout << "Part 2: " << sumIncorrect << "\n";
}

int main() {
  std::ifstream ifs("input.txt");
  //std::ifstream ifs("input_test.txt");
  auto data = parseFile(ifs);
  measureTime([&data]() { solvePart1And2(data); }, "Part 1 + 2");
  std::cout << "Done.\n";
  return 0;
}