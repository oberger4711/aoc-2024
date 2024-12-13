#include "utils.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>

using num = long long;

struct Data {
  std::vector<num> stones;
};

Data parseFile(std::ifstream &ifs) {
  Data data;
  data.stones = parseCSVNumbers<num>(ifs, ' ')[0];
  return data;
}

void printStones(const std::vector<num> &stones) {
  for (num s : stones) {
    std::cout << s << " ";
  }
  std::cout << "\n";
}

int getNumDigits(num n) {
  if (n == 0) {
    return 1;
  }
  return std::log10(n) + 1;
}

std::pair<num, num> splitStone(num n, int rightDigits) {
  const num div = std::pow(10, rightDigits);
  return std::pair<num, num>(n / div, n % div);
}

void blink(const std::vector<num> &stones, std::vector<num> &nextStones) {
  // nextStones.reserve(stones.size());
  nextStones.clear();
  for (size_t i = 0; i < stones.size(); ++i) {
    const auto &stone = stones[i];
    if (stone == 0) {
      nextStones.push_back(1);
    } else {
      const int digits = getNumDigits(stone);
      if (digits % 2 == 0) {
        num a, b;
        std::tie(a, b) = splitStone(stone, digits / 2);
        nextStones.push_back(a);
        nextStones.push_back(b);
      } else {
        nextStones.push_back(stone * 2024);
      }
    }
  }
}

// This uses a naive algorithm.
// Part 2 uses the faster algorithm.
void solvePart1(const Data &data) {
  std::vector<num> stones = data.stones;
  std::vector<num> nextStones;
  for (int round = 0; round < 25; ++round) {
    blink(stones, nextStones);
    std::swap(stones, nextStones);
    // std::cout << "Blinks: " << round + 1 << ": " << stones.size() << "\n";
    //  printStones(stones);
    //  std::cout << "\n";
  }
  std::cout << stones.size() << "\n";
}

// Data structure requires some forward declaration because of recursion:
struct Power;
struct Element;
using Memory = std::unordered_map<num, std::unique_ptr<Power>>;

struct Element {
  Element() = default;
  Element(Memory &memory, num base, int iteration);
  void iterate(Memory &memory);

  Power *power;
  int iteration;
  num count;
};
Element getElement(Memory &memory, num base, int iteration);

struct Power {
  Power() = default;
  Power(num base_) : base(base_) {
    knownIterations.push_back(1); // Single stone at iteration 0.
  }
  num getCount(Memory &memory, int iteration);
  void iterate(Memory &memory);

  num base;
  std::vector<num> knownIterations; // iteration -> count
  // TOOD: Try 2 std::optionals which does not use allocation. Might be faster.
  std::vector<Element> lastIterationElements;
};

Element::Element(Memory &memory, num base, int iteration_)
    : iteration(iteration_) {
  auto &powerPtr = memory[base];
  if (powerPtr == nullptr) {
    powerPtr = std::make_unique<Power>(base);
  }
  power = powerPtr.get();
  count = powerPtr->getCount(memory, iteration);
}

void Element::iterate(Memory &memory) {
  ++iteration;
  count = power->getCount(memory, iteration);
}

num Power::getCount(Memory &memory, int iteration) {
  while (knownIterations.size() <= iteration) {
    iterate(memory);
  }
  return knownIterations[iteration];
}

void Power::iterate(Memory &memory) {
  int iteration = knownIterations.size();
  num count = 0;
  if (iteration == 1) {
    auto &nextElements = lastIterationElements;
    if (base == 0) {
      nextElements.emplace_back(memory, 1, 0);
      count = 1;
    } else {
      const int digits = getNumDigits(base);
      if (digits % 2 == 0) {
        num a, b;
        std::tie(a, b) = splitStone(base, digits / 2);
        nextElements.emplace_back(memory, a, 0);
        nextElements.emplace_back(memory, b, 0);
        count = 2;
      } else {
        nextElements.emplace_back(memory, base * 2024, 0);
        count = 1;
      }
    }
  } else {
    for (auto &e : lastIterationElements) {
      e.iterate(memory);
      count += e.count;
    }
  }
  knownIterations.push_back(count);
}

std::vector<Element> makeInitialElements(Memory &memory,
                                         const std::vector<num> initialStones) {
  std::vector<Element> ret;
  std::transform(initialStones.begin(), initialStones.end(),
                 std::back_inserter(ret),
                 [&memory](num n) { return Element(memory, n, 0); });
  return ret;
}

void solvePart2(const Data &data) {
  Memory memory;
  auto elements = makeInitialElements(memory, data.stones);
  for (int round = 0; round < 75; ++round) {
    for (auto &e : elements) {
      e.iterate(memory);
    }
  }
  num count = 0;
  for (auto &e : elements) {
    count += e.count;
  }
  std::cout << count << "\n";
  // Correct: 218279375708592
  // vector: 35 ms
}

void test() {
  assert(getNumDigits(0) == 1);
  assert(getNumDigits(1) == 1);
  assert(getNumDigits(9) == 1);
  assert(getNumDigits(10) == 2);
  assert(getNumDigits(11) == 2);
  assert(getNumDigits(19) == 2);
  assert(getNumDigits(99) == 2);
  assert(getNumDigits(100) == 3);
  assert(getNumDigits(999) == 3);
  assert(getNumDigits(1000) == 4);
  assert(getNumDigits(10000000) == 8);
  assert(splitStone(123456, 3).first == 123);
  assert(splitStone(123456, 3).second == 456);
  assert(splitStone(90, 1).first == 9);
  assert(splitStone(90, 1).second == 0);
  assert(splitStone(10, 1).first == 1);
  assert(splitStone(10, 1).second == 0);
  assert(splitStone(99910000, 4).first == 9991);
  assert(splitStone(99910000, 4).second == 0);
}

int main() {
  // test();
  std::ifstream ifs("input.txt");
  // std::ifstream ifs("input_test1.txt");
  //  std::ifstream ifs("input_test.txt");
  const auto data = parseFile(ifs);
  measureTime([&data]() { solvePart1(data); }, "Part 1");
  measureTime([&data]() { solvePart2(data); }, "Part 2");
  std::cout << "Done.\n";
  return 0;
}