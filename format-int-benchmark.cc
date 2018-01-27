#include <algorithm>
#include <cmath>
#include <cstdio>
#include <limits>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <benchmark/benchmark.h>
#include <fmt/format.h>

// Returns a pair with the smallest and the largest value of integral type T
// with the given number of digits.
template <typename T>
std::pair<T, T> range(int num_digits) {
  T first = std::pow(T(10), num_digits - 1);
  int max_digits = std::numeric_limits<T>::digits10 + 1;
  T last = num_digits < max_digits ? first * 10 - 1 :
	                             std::numeric_limits<T>::max();
  return {num_digits > 1 ? first : 0, last};
}

// Generates values of integral type T with random number of digits.
template <typename T>
std::vector<T> generate_random_data(int numbers_per_digit) {
  int max_digits = std::numeric_limits<T>::digits10 + 1;
  std::vector<T> data;
  data.reserve(max_digits * numbers_per_digit);
  for (int i = 1; i <= max_digits; ++i) {
    auto r = range<T>(i);
    auto value = r.first;
    std::generate_n(std::back_inserter(data), numbers_per_digit, [=]() mutable {
      T result = value;
      value = value < r.second ? value + 1 : r.first;
      return result;
    });
  }
  std::random_shuffle(data.begin(), data.end());
  return data;
}

//TODO: test

auto data = generate_random_data<int>(1000);

void sprintf(benchmark::State &s) {
  size_t result = 0;
  while (s.KeepRunning()) {
    for (auto i: data) {
      char buffer[12];
      result += std::sprintf(buffer, "%d", i);
    }
  }
  benchmark::DoNotOptimize(result);
}
BENCHMARK(sprintf);

void ostringstream(benchmark::State &s) {
  size_t result = 0;
  while (s.KeepRunning()) {
    for (auto i: data) {
      std::ostringstream ss;
      ss << i;
      result += ss.str().size();
    }
  }
  benchmark::DoNotOptimize(result);
}
BENCHMARK(ostringstream);

void to_string(benchmark::State &s) {
  size_t result = 0;
  while (s.KeepRunning()) {
    for (auto i: data)
      result += std::to_string(i).size();
  }
  benchmark::DoNotOptimize(result);
}
BENCHMARK(to_string);

void format(benchmark::State &s) {
  size_t result = 0;
  while (s.KeepRunning()) {
    for (auto i: data)
      result += fmt::format("{}", i).size();
  } 
  benchmark::DoNotOptimize(result);
}
BENCHMARK(format);

void format_to(benchmark::State &s) {
  size_t result = 0;
  while (s.KeepRunning()) {
    for (auto i: data) {
      char buffer[12];
      result += fmt::format_to(buffer, "{}", i) - buffer;
    }
  }
  benchmark::DoNotOptimize(result);
}
BENCHMARK(format_to);

BENCHMARK_MAIN();
