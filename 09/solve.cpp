#include "utils.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <list>
#include <string>

using num = long long;

struct Data {
  std::string line;
};

Data parseFile(std::ifstream &ifs) {
  Data data;
  assert(std::getline(ifs, data.line));
  return data;
}

struct File {
  File() = default;
  File(int id_, num start_, num size_) : id(id_), start(start_), size(size_) {}

  int id;
  num start;
  num size;
};

template <typename Container> void printFiles(const Container &files) {
  for (const auto &f : files) {
    std::cout << f.id << ": [" << f.start << ", " << f.start + f.size - 1
              << "]\n";
  }
}

std::vector<File> inputToFiles(const Data &data) {
  std::vector<File> files;
  bool chunkIsFile = true;
  num pos = 0;
  int id = 0;
  for (int i = 0; i < data.line.size(); ++i) {
    const num chunkSize = data.line[i] - '0';
    if (chunkIsFile) {
      if (chunkSize > 0) {
        files.emplace_back(id, pos, chunkSize);
      }
      ++id;
    }
    pos += chunkSize;
    chunkIsFile = !chunkIsFile;
  }
  return files;
}

num sumSeq(num start, num end) {
  return (end * (end - 1) - start * (start - 1)) / 2;
}

template <typename Container> num computeCheckSum(const Container &files) {
  num checkSum = 0;
  for (const auto &file : files) {
    auto prod = file.id * sumSeq(file.start, file.start + file.size);
    checkSum += prod;
  }
  return checkSum;
}

void solvePart1(const Data &data) {
  auto fragmentedFiles = inputToFiles(data);
  std::vector<File> defragmentedFiles;
  defragmentedFiles.reserve(fragmentedFiles.size());
  num insertPos = 0;
  for (size_t i = 0; i < fragmentedFiles.size(); ++i) {
    auto &nextFragmentedFile = fragmentedFiles[i];
    num freeSpace = nextFragmentedFile.start - insertPos;
    while (fragmentedFiles.size() > i && freeSpace > 0) {
      auto &backFragmentedFile = fragmentedFiles.back();
      num occupyingSpace = std::min(freeSpace, backFragmentedFile.size);
      defragmentedFiles.push_back(
          {backFragmentedFile.id, insertPos, occupyingSpace});
      freeSpace -= occupyingSpace;
      backFragmentedFile.size -= occupyingSpace;
      insertPos += occupyingSpace;
      if (backFragmentedFile.size == 0) {
        fragmentedFiles.pop_back();
      }
    }
    if (fragmentedFiles.size() > i) {
      defragmentedFiles.push_back(nextFragmentedFile);
      insertPos += nextFragmentedFile.size;
    }
  }
  // printFiles(defragmentedFiles);
  std::cout << computeCheckSum(defragmentedFiles) << "\n";
}

// This is really not a fast solution.
// But I am already behind one day so I keep this.
void solvePart2(const Data &data) {
  auto fragmentedFiles = inputToFiles(data);
  std::list<File> files;
  std::copy(fragmentedFiles.begin(), fragmentedFiles.end(),
            std::back_inserter(files));
  for (auto srcIt = files.rbegin(); srcIt != files.rend();) {
    auto srcItFwd = std::next(srcIt).base();
    bool erased = false;
    for (auto destPrevIt = files.begin(); destPrevIt != srcItFwd;
         ++destPrevIt) {
      auto destIt = std::next(destPrevIt);
      num freeSpace = destIt->start - (destPrevIt->start + destPrevIt->size);
      if (freeSpace >= srcIt->size) {
        files.insert(
            destIt,
            File(srcIt->id, destPrevIt->start + destPrevIt->size, srcIt->size));
        auto erasedIt = files.erase(srcItFwd);
        srcIt = std::make_reverse_iterator(erasedIt);
        erased = true;
        break;
      }
    }
    if (!erased) {
      ++srcIt;
    }
  }
  // printFiles(files);
  std::cout << computeCheckSum(files) << "\n";
}

int main() {
  std::ifstream ifs("input.txt");
  // std::ifstream ifs("input_test.txt");
  const auto data = parseFile(ifs);
  measureTime([&data]() { solvePart1(data); }, "Part 1");
  measureTime([&data]() { solvePart2(data); }, "Part 2");
  std::cout << "Done.\n";
  return 0;
}