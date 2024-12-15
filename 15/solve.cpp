#include "utils.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using num = int;
using Coords = Coords_<num>;

struct Data {
  std::vector<std::string> lines;
  Coords robotPos;
  std::string moves;
};

Data parseFile(std::ifstream &ifs) {
  Data data;
  data.robotPos.row = -1; // Indicates robot not yet found.
  std::string line;
  while (std::getline(ifs, line) && !line.empty()) {
    if (data.robotPos.row == -1) {
      // Search for the robot.
      const auto col = line.find('@');
      if (col != std::string::npos) {
        line[col] = '.'; // Replace with free space.
        data.robotPos = Coords(data.lines.size(), col);
      }
    }
    data.lines.push_back(line);
  }
  assert(!data.lines.empty());
  std::stringstream ss;
  while (std::getline(ifs, line)) {
    ss << line;
  }
  data.moves = ss.str();
  assert(data.robotPos.row >= 0);
  return data;
}

struct Map {
  Map(std::vector<std::string> lines_)
      : lines(std::move(lines_)), size(lines.size(), lines[0].size()) {}

  std::vector<std::string> lines;
  Coords size;
  char &operator()(const Coords &coords) {
    return lines[coords.row][coords.col];
  }
  const char operator()(const Coords &coords) const {
    return lines[coords.row][coords.col];
  }
  char &operator()(num row, num col) { return lines[row][col]; }
  const char operator()(num row, num col) const { return lines[row][col]; }
};

struct State {
  Map map;
  Coords robotPos;
};

Coords charToDir(char move) {
  if (move == '<') {
    return Coords::Left();
  } else if (move == 'v') {
    return Coords::Down();
  } else if (move == '>') {
    return Coords::Right();
  } else if (move == '^') {
    return Coords::Up();
  } else {
    std::cout << "Unknown move '" << move << "'!\n";
    return Coords(0, 0);
  }
}

bool move1(State &state, const Coords &pos, const Coords &dir) {
  char ch = state.map(pos);
  if (ch == '#') {
    // Immovable.
    return false;
  } else if (ch == '.') {
    return true;
  } else if (ch == 'O') {
    // Box in the way.
    // Can we move it?
    const Coords nextPos = pos + dir;
    if (move1(state, nextPos, dir)) {
      state.map(pos) = '.';
      state.map(nextPos) = 'O';
      return true;
    } else {
      return false;
    }
  } else {
    std::cout << "Unknown char '" << ch << "' in map.\n";
    return false;
  }
}

template <typename MoveFunc>
void iterate(State &state, Coords dir, MoveFunc moveFunc) {
  const Coords nextRobotPos = state.robotPos + dir;
  if (moveFunc(state, nextRobotPos, dir)) {
    state.robotPos = nextRobotPos;
  }
}

num sumGPSCoords(const State &state) {
  num sum = 0;
  for (num row = 0; row < state.map.size.row; ++row) {
    const auto &line = state.map.lines[row];
    for (num col = 0; col < line.size(); ++col) {
      const char ch = line[col];
      if (ch == 'O' || ch == '[') {
        sum += row * 100 + col;
      }
    }
  }
  return sum;
}

void solvePart1(const Data &data) {
  State state{Map(data.lines), data.robotPos};
  auto &moves = data.moves;
  for (num i = 0; i < moves.size(); ++i) {
    const Coords dir = charToDir(moves[i]);
    iterate(state, dir, move1);
  }
  std::cout << sumGPSCoords(state) << "\n";
}

Map makeWideMap(const Data &data) {
  std::vector<std::string> widerLines;
  widerLines.reserve(data.lines.size());
  for (const auto &line : data.lines) {
    std::stringstream ss;
    for (char ch : line) {
      if (ch == 'O') {
        ss << "[]";
      } else {
        ss << ch << ch;
      }
    }
    widerLines.push_back(ss.str());
  }
  return Map(widerLines);
}

Coords makeWidePos(const Coords &pos) { return Coords(pos.row, pos.col * 2); }

bool canMoveBox(const State &state, const Coords &leftPos, const Coords &dir,
                std::vector<Coords> *leftCoordsToMove);

bool canMove(const State &state, const Coords &pos, const Coords &dir,
             std::vector<Coords> *leftCoordsToMove) {
  const char ch = state.map(pos);
  if (ch == '#') {
    leftCoordsToMove->clear();
    return false;
  } else if (ch == '.') {
    return true;
  } else if (ch == '[') {
    return canMoveBox(state, pos, dir, leftCoordsToMove);
  } else if (ch == ']') {
    return canMoveBox(state, Coords(pos.row, pos.col - 1), dir,
                      leftCoordsToMove);
  } else {
    std::cout << "Unknown char '" << ch << "' in map.\n";
    return false;
  }
}

bool canMoveBox(const State &state, const Coords &leftPos, const Coords &dir,
                std::vector<Coords> *leftCoordsToMove) {
  leftCoordsToMove->push_back(leftPos);
  if (dir.col > 0) {
    const Coords rightPos = Coords(leftPos.row, leftPos.col + 1);
    return canMove(state, rightPos + dir, dir, leftCoordsToMove);
  } else if (dir.row != 0) {
    // Up / down
    const Coords rightPos = Coords(leftPos.row, leftPos.col + 1);
    return canMove(state, leftPos + dir, dir, leftCoordsToMove) &&
           canMove(state, rightPos + dir, dir, leftCoordsToMove);
  } else {
    // Right
    return canMove(state, leftPos + dir, dir, leftCoordsToMove);
  }
}

void doMoveCheckedBoxes(State &state,
                        const std::vector<Coords> &leftCoordsToMove,
                        const Coords &dir) {
  // std::cout << "Moving " << leftCoordsToMove.size() << " boxes.\n";
  for (const auto &leftPos : leftCoordsToMove) {
    Coords nextPos = leftPos + dir;
    state.map(leftPos) = '.';
    state.map(leftPos.row, leftPos.col + 1) = '.';
  }
  for (const auto &leftPos : leftCoordsToMove) {
    Coords nextPos = leftPos + dir;
    state.map(nextPos) = '[';
    state.map(nextPos.row, nextPos.col + 1) = ']';
  }
}

bool move2(State &state, const Coords &pos, const Coords &dir) {
  const char ch = state.map(pos);
  if (ch == '#') {
    // Immovable.
    return false;
  } else if (ch == '.') {
    return true;
  } else {
    // Box in the way.
    Coords leftPos = pos;
    if (ch == ']') {
      --leftPos.col;
    }
    // Can we move it?
    std::vector<Coords> leftCoordsToMove;
    if (canMove(state, leftPos, dir, &leftCoordsToMove)) {
      doMoveCheckedBoxes(state, leftCoordsToMove, dir);
      return true;
    } else {
      return false;
    }
  }
}

void solvePart2(const Data &data) {
  State state{makeWideMap(data), makeWidePos(data.robotPos)};
  for (num i = 0; i < data.moves.size(); ++i) {
    const Coords dir = charToDir(data.moves[i]);
    iterate(state, dir, move2);
    // std::cout << state.map.lines;
    // std::cout << i + 1 << "\n\n";
  }
  std::cout << sumGPSCoords(state) << "\n";
}

int main() {
  std::ifstream ifs("input.txt");
  // std::ifstream ifs("input_test.txt");
  // std::ifstream ifs("input_test1.txt");
  // std::ifstream ifs("input_test2.txt");
  const auto data = parseFile(ifs);
  measureTime([&data]() { solvePart1(data); }, "Part 1");
  measureTime([&data]() { solvePart2(data); }, "Part 2");
  std::cout << "Done.\n";
  return 0;
}