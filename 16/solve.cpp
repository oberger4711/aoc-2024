#include "utils.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <queue>
#include <set>
#include <string>
#include <unordered_map>

using num = int;
using Coords = Coords_<num>;

struct Data {
  std::vector<std::string> lines;
  Coords start;
  Coords end;
};

void searchCharAndMarkFree(std::string *line, num row, char ch,
                           Coords *coords) {
  if (coords->row < 0) {
    const auto pos = line->find(ch);
    if (pos != std::string::npos) {
      line->operator[](pos) = '+'; // Mark as crossing so that we stop here.
      coords->row = row;
      coords->col = pos;
    }
  }
}

bool isFree(char ch) { return ch == '.' || ch == '+'; }

Data parseFile(std::ifstream &ifs) {
  Data data;
  std::string line;
  data.start.row = -1;
  data.end.row = -1;
  num row = 0;
  while (std::getline(ifs, line)) {
    searchCharAndMarkFree(&line, row, 'S', &data.start);
    searchCharAndMarkFree(&line, row, 'E', &data.end);
    data.lines.push_back(line);
    ++row;
  }
  data.lines[data.start.row][data.start.col] = '.';
  data.lines[data.end.row][data.end.col] = '.';
  // For easier and quicker search later on:
  // Mark positions at which turning makes sense.
  for (num row = 1; row + 1 < data.lines.size(); ++row) {
    for (num col = 1; col + 1 < data.lines.size(); ++col) {
      if (data.lines[row][col] == '.') {
        const bool leftFree = isFree(data.lines[row][col - 1]);
        const bool rightFree = isFree(data.lines[row][col + 1]);
        const bool upFree = isFree(data.lines[row - 1][col]);
        const bool downFree = isFree(data.lines[row + 1][col]);
        int numFree = static_cast<int>(leftFree) + static_cast<int>(rightFree) +
                      static_cast<int>(upFree) + static_cast<int>(downFree);
        // Do not try to turn in dead ends or on straights.
        if (numFree != 1 && !(numFree == 2 && ((leftFree && rightFree) ||
                                               (upFree && downFree)))) {
          data.lines[row][col] = '+';
        }
      }
    }
  }
  return data;
}

struct Pose {
  static const std::array<Coords, 4> DIRECTIONS;
  Pose() = default;
  Pose(const Coords &pos_) : pos(pos_) {}
  Pose(const Coords &pos_, char dirIndex_) : pos(pos_), dirIndex(dirIndex_) {}

  Coords pos;
  char dirIndex = 0;
  Coords getDir() const { return Pose::DIRECTIONS[dirIndex]; }
  char getNextDirIndex() const { return (dirIndex + 1) % 4; }
  char getPrevDirIndex() const {
    char prev = dirIndex - 1;
    if (prev == -1) {
      prev = 3;
    }
    return prev;
  }
  Pose goForward() const { return Pose(pos + getDir(), dirIndex); }
  Pose goLeft() const { return Pose(pos, getNextDirIndex()); }
  Pose goRight() const { return Pose(pos, getPrevDirIndex()); }
};

// For usage with std::unordered_map.
bool operator==(const Pose &lhs, const Pose &rhs) {
  return lhs.pos == rhs.pos && lhs.dirIndex == rhs.dirIndex;
}

// For usage with std::unordered_map.
struct PoseHash {
  auto operator()(const Pose &p) const -> size_t {
    return std::hash<num>{}(p.pos.row) ^ std::hash<num>{}(p.pos.col) ^
           std::hash<char>{}(p.dirIndex);
  }
};

const std::array<Coords, 4> Pose::DIRECTIONS = {Coords::Right(), Coords::Up(),
                                                Coords::Left(), Coords::Down()};

struct Node {
  Node() = default;
  Node(const Pose &pose_, num accumulatedCosts_, num estimatedRemainingCosts_)
      : pose(pose_), accCosts(accumulatedCosts_),
        estimatedCosts(accumulatedCosts_ + estimatedRemainingCosts_) {}

  Pose pose;
  num accCosts;
  num estimatedCosts;
  // std::vector<Pose> trajectory;
};

bool operator<(const Node &lhs, const Node &rhs) {
  // Actually this should return lhs < rhs.
  // But I want the lowest costs to be top in the prio queue.
  return rhs.accCosts < lhs.accCosts;
}

struct Record {
  Record() = default;
  Record(num bestAccCosts_) : bestAccCosts(bestAccCosts_) {}

  num bestAccCosts;
  std::set<Coords> bestPredecessors;
};

class AStar {
public:
  void solvePart1(const Data &data) {
    end = data.end;
    const Pose startPose(data.start);
    q.push(Node(startPose, 0, heuristic1(startPose)));
    reachedPoses[startPose] = Record(0);
    num bestCostsForSolution = std::numeric_limits<num>::max();
    std::vector<Node> solutions;
    long long it = 0;
    while (true) {
      const Node n = q.top();
      // if ((it++) % 100 == 0) {
      //   std::cout << n.accCosts << "\n";
      // }
      q.pop();
      if (n.accCosts > bestCostsForSolution) {
        // Found all best solutions.
        break;
      }
      if (n.pose.pos == data.end) {
        std::cout << "solution\n";
        // Reached goal.
        bestCostsForSolution = std::min(bestCostsForSolution, n.accCosts);
        solutions.push_back(n);
        // Keep searching for other best solutions.
      } else {
        // Expand node.
        const auto &predecessors = reachedPoses[n.pose].bestPredecessors;
        {
          // Forward
          Pose neighPose = n.pose;
          num neighAccCosts = n.accCosts;
          std::set<Coords> movedCoords;
          do {
            neighPose = neighPose.goForward();
            ++neighAccCosts;
            movedCoords.insert(neighPose.pos);
            const char neighCh =
                data.lines[neighPose.pos.row][neighPose.pos.col];
            if (neighCh == '#') {
              // Dead end
              // Not worth putting it into queue.
              break;
            } else if (neighCh == '+') {
              // Reached a crossing -> new node.
              tryEnqueueNode(neighPose, neighAccCosts, predecessors,
                             movedCoords);
              break;
            }
          } while (true);
        }
        {
          // Rotate ccwise.
          Pose neighPose = n.pose;
          num neighAccCosts = n.accCosts;
          for (int i = 0; i < 2; ++i) {
            neighPose = n.pose.goLeft();
            neighAccCosts = n.accCosts + 1000;
            tryEnqueueNode(neighPose, neighAccCosts, predecessors);
          }
        }
        {
          // Rotate cwise.
          const Pose neighPose = n.pose.goRight();
          const num neighAccCosts = n.accCosts + 1000;
          tryEnqueueNode(neighPose, neighAccCosts, predecessors);
        }
      }
    }
    assert(!solutions.empty());
    std::cout << solutions[0].accCosts << "\n";
    // Merge pathes to end pose with any direction.
    std::set<Coords> bestCoords;
    for (int i = 0; i < 4; ++i) {
      auto &predecessors = reachedPoses[Pose(end, i)].bestPredecessors;
      bestCoords.insert(predecessors.begin(), predecessors.end());
    }
    std::cout << bestCoords.size() + 1 << "\n";
  }

private:
  void tryEnqueueNode(const Pose &pose, num accCosts,
                      const std::set<Coords> &predecessors,
                      const std::set<Coords> nextCoords = std::set<Coords>()) {
    auto inserted = reachedPoses.insert(std::make_pair(pose, Record(accCosts)));
    auto &it = inserted.first;
    auto &record = it->second;
    num &knownCosts = record.bestAccCosts;
    if (inserted.second || accCosts < knownCosts) {
      // Better trajectory found.
      record.bestPredecessors = predecessors;
      record.bestPredecessors.insert(nextCoords.begin(), nextCoords.end());
      knownCosts = accCosts;
      q.emplace(pose, accCosts, heuristic1(pose));
    } else if (knownCosts == accCosts) {
      // Another best trajectory to this next node.
      record.bestPredecessors.insert(predecessors.begin(), predecessors.end());
      record.bestPredecessors.insert(nextCoords.begin(), nextCoords.end());
    }
  }

  num heuristic1(const Pose &pose) {
    num dist = (end - pose.pos).manhattanDistance();
    if (pose.pos.row != end.row && pose.pos.col != end.col) {
      // We need at least one more turn.
      dist += 1000;
    }
    return dist;
  }

  Coords end;
  std::priority_queue<Node> q;
  std::unordered_map<Pose, Record, PoseHash> reachedPoses; // -> min costs
};

void solvePart1And2(const Data &data) {
  AStar astar;
  astar.solvePart1(data);
}

int main() {
  std::ifstream ifs("input.txt");
  // std::ifstream ifs("input_test.txt");
  // std::ifstream ifs("input_test1.txt");
  const auto data = parseFile(ifs);
  measureTime([&data]() { solvePart1And2(data); }, "Part 1 + 2");
  std::cout << "Done.\n";
  return 0;
}