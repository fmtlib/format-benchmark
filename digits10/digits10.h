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
