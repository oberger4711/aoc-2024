#include <chrono>
#include <iomanip>
#include <iostream>
#include <vector>

// This file is copied into each day.
// That is redundant but this way I can still run older solutions without having
// to touch them when I make changes in the utils.h.

// Use this in the solve.cpp:
// using Coords = Coords_<int>;
// or whatever type fits the puzzle best.
template <typename T> struct Coords_ {
  T row, col;
  Coords_(T row_, T col_) : row(row_), col(col_) {}
  Coords_<T> plus(T row_, T col_) { return Coords_<T>(row + row_, col + col_); }
  void plusEquals(T row_, T col_) {
    row += row_;
    col += col_;
  }
  Coords_<T> operator+(const Coords_<T> &rhs) const {
    return Coords_<T>(row + rhs.row, col + rhs.col);
  }
  Coords_<T> operator-(const Coords_<T> &rhs) const {
    return Coords_<T>(row - rhs.row, col - rhs.col);
  }
  Coords_<T> &operator+=(const Coords_<T> &rhs) {
    row += rhs.row;
    col += rhs.col;
    return *this;
  }
  Coords_<T> &operator-=(const Coords_<T> &rhs) {
    row -= rhs.row;
    col -= rhs.col;
    return *this;
  }
  Coords_<T> operator*(const T scale) const {
    return Coords_<T>(row * scale, col * scale);
  }
  Coords_<T> &operator*=(const T scale) {
    row *= scale;
    col *= scale;
    return *this;
  }
  T manhattanDistance() const { return std::abs(row) + std::abs(col); }

  static Coords_<T> Up() { return Coords_<T>(-1, 0); }
  static Coords_<T> Down() { return Coords_<T>(1, 0); }
  static Coords_<T> Left() { return Coords_<T>(0, -1); }
  static Coords_<T> Right() { return Coords_<T>(0, 1); }
};

template <typename T>
inline bool operator==(const Coords_<T> &lhs, const Coords_<T> &rhs) {
  return lhs.row == rhs.row && lhs.col == rhs.col;
}

inline bool isDigit(char ch) { return 0x30 <= ch && ch < 0x3A; }

template <typename Func>
void measureTime(Func func, const std::string &name, int repetitions = 1) {
  std::cout << "Running " << name << "...\n";
  const auto start = std::chrono::steady_clock::now();
  for (int i = 0; i < repetitions; ++i) {
    func();
  }
  const auto end = std::chrono::steady_clock::now();
  const auto diff = end - start;
  // Print duration in readable form.
  std::cout << name << " took ";
  if (std::chrono::duration_cast<std::chrono::milliseconds>(diff).count() /
          static_cast<double>(repetitions) >=
      10000.0) {
    const auto s =
        (std::chrono::duration_cast<std::chrono::milliseconds>(diff).count() /
         static_cast<double>(repetitions)) /
        1000.0;
    std::cout << std::setprecision(6) << s << " s\n";
  }
  if (std::chrono::duration_cast<std::chrono::milliseconds>(diff).count() /
          static_cast<double>(repetitions) <
      10.0) {
    const auto ms =
        (std::chrono::duration_cast<std::chrono::microseconds>(diff).count() /
         static_cast<double>(repetitions)) /
        1000.0;
    std::cout << std::setprecision(6) << ms << " ms\n";

  } else {
    std::cout << std::setprecision(6)
              << (std::chrono::duration_cast<std::chrono::milliseconds>(diff)
                      .count() /
                  static_cast<double>(repetitions))
              << " ms\n";
  }
}

inline std::ostream &operator<<(std::ostream &os,
                                const std::vector<std::string> &lines) {
  for (const auto &line : lines) {
    os << line << "\n";
  }
  return os;
}
