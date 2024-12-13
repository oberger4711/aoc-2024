#include "utils.h"
#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <tuple>
#include <unordered_map>

constexpr int ITERATIONS_PART_1 = 25;
constexpr int ITERATIONS_PART_2 = 75;

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

void blink(const std::vector<num> &stones, std::vector<num> &nextStones) {
  nextStones.clear();
  for (size_t i = 0; i < stones.size(); ++i) {
    const auto &stone = stones[i];
    if (stone == 0) {
      nextStones.push_back(1);
    } else {
      const int digits = getNumDigits(stone);
      if (digits % 2 == 0) {
        num a, b;
        std::tie(a, b) = splitNumber(stone, digits / 2);
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
  for (int round = 0; round < ITERATIONS_PART_1; ++round) {
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
  Power(num base_) : base(base_), knownIterationsSize(1) {
    knownIterations[0] = 1; // Single stone at iteration 0.
  }
  num getCount(Memory &memory, int iteration);
  void iterate(Memory &memory);

  num base;
  // Array is faster than vector here. I got around 40 % speed up.
  // This is probably because it saves the allocation, indirection and or
  // improves caching.
  // With C++ 26 we might better use the std::inplace_vector<>.
  std::array<num, ITERATIONS_PART_2 + 1> knownIterations; // iteration -> count
  unsigned short knownIterationsSize;
  // This could also be an array or small vector.
  // It is also slightly faster to have this inplace apparently.
  std::optional<Element> firstChildElement;
  std::optional<Element> secondChildElement;
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
  while (knownIterationsSize <= iteration) {
    iterate(memory);
  }
  return knownIterations[iteration];
}

void Power::iterate(Memory &memory) {
  int iteration = knownIterationsSize;
  num count = 0;
  if (iteration == 1) {
    if (base == 0) {
      firstChildElement.emplace(memory, 1, 0);
      count = 1;
    } else {
      const int digits = getNumDigits(base);
      if (digits % 2 == 0) {
        num a, b;
        std::tie(a, b) = splitNumber(base, digits / 2);
        firstChildElement.emplace(memory, a, 0);
        secondChildElement.emplace(memory, b, 0);
        count = 2;
      } else {
        firstChildElement.emplace(memory, base * 2024, 0);
        count = 1;
      }
    }
  } else {
    if (firstChildElement) {
      firstChildElement->iterate(memory);
      count += firstChildElement->count;
    }
    if (secondChildElement) {
      secondChildElement->iterate(memory);
      count += secondChildElement->count;
    }
  }
  knownIterations[iteration] = count;
  ++knownIterationsSize;
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
  for (int round = 0; round < ITERATIONS_PART_2; ++round) {
    for (auto &e : elements) {
      e.iterate(memory);
    }
  }
  num count = 0;
  for (auto &e : elements) {
    count += e.count;
  }
  std::cout << count << "\n";
}

// Some DIY unit tests.
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
  assert(splitNumber(123456, 3).first == 123);
  assert(splitNumber(123456, 3).second == 456);
  assert(splitNumber(90, 1).first == 9);
  assert(splitNumber(90, 1).second == 0);
  assert(splitNumber(10, 1).first == 1);
  assert(splitNumber(10, 1).second == 0);
  assert(splitNumber(99910000, 4).first == 9991);
  assert(splitNumber(99910000, 4).second == 0);
}

int main() {
  // test();
  std::ifstream ifs("input.txt");
  // std::ifstream ifs("input_test.txt");
  // std::ifstream ifs("input_test1.txt");
  const auto data = parseFile(ifs);
  measureTime([&data]() { solvePart1(data); }, "Part 1");
  measureTime([&data]() { solvePart2(data); }, "Part 2");
  std::cout << "Done.\n";
  return 0;
}