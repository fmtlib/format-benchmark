#ifndef DIGITS10_H_
#define DIGITS10_H_

#include <cmath>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

#include "benchmark/benchmark.h"

inline std::uint32_t digits10_naive(std::uint32_t n) {
  std::uint32_t result = 0;
  do {
    ++result;
    n /= 10;
  } while (n);
  return result;
}

inline std::uint32_t digits10_unroll4(std::uint32_t n) {
  std::uint32_t result = 1;
  for (;;) {
    if (n < 10)    return result;
    if (n < 100)   return result + 1;
    if (n < 1000)  return result + 2;
    if (n < 10000) return result + 3;
    n /= 10000u;
    result += 4;
  }
}

extern const uint32_t powers_of_10_u32[];

inline std::uint32_t digits10_clz(std::uint32_t n) {
  std::uint32_t t = (32 - __builtin_clz(n | 1)) * 1233 >> 12;
  return t - (n < powers_of_10_u32[t]) + 1;
}

// Maps the result of bsr(n) to ceil(log10(n)).
static const uint16_t bsr2log10[] = {
  1, 1, 1,
  2, 2, 2,
  3, 3, 3,
  4, 4, 4, 4,
  5, 5, 5,
  6, 6, 6,
  7, 7, 7, 7,
  8, 8, 8,
  9, 9, 9,
  10, 10, 10
};

static const uint32_t powers_of_10_u32_z[] = {
  0,
  0,
  10,
  100,
  1000,
  10000,
  100000,
  1000000,
  10000000,
  100000000,
  1000000000
};

// My version of digits10_clz that converts clz to bsr and uses two lookup
// tables.
inline std::uint32_t digits10_clz_zverovich(std::uint32_t n) {
  auto t = bsr2log10[__builtin_clz(n | 1) ^ 31];
  return t - (n < powers_of_10_u32_z[t]);
}

inline int digits10_grisu(uint32_t n) {
  if (n < 10) return 1;
  if (n < 100) return 2;
  if (n < 1000) return 3;
  if (n < 10000) return 4;
  if (n < 100000) return 5;
  if (n < 1000000) return 6;
  if (n < 10000000) return 7;
  if (n < 100000000) return 8;
  if (n < 1000000000) return 9;
  return 10;
}

// Return minimum number with the specified number of digits.
inline std::uint32_t min_number(unsigned num_digits) {
  if (num_digits == 0 || num_digits > 10)
    throw std::out_of_range("num_digits is out of range");
  return num_digits == 1 ? 0 : std::pow(10, num_digits - 1);
}

inline std::uint32_t max_number(unsigned num_digits) {
  if (num_digits == 0 || num_digits > 10)
    throw std::out_of_range("num_digits is out of range");
  return num_digits == 10 ?
     std::numeric_limits<uint32_t>::max() : std::pow(10, num_digits) - 1;
}

// Generate 100 numbers with specified number of digits.
std::vector<std::uint32_t> generate_numbers(int num_digits);

template <typename F>
void run_benchmark(benchmark::State& state, F digits10) {
  int num_digits = state.range();
  auto numbers = generate_numbers(num_digits);
  bool valid = true;
  while (state.KeepRunning()) {
    for (auto n: numbers)
      valid &= (digits10(n) == num_digits);
  }
  if (!valid)
    throw std::logic_error("invalid result");
}

#endif  // DIGITS10_H_
