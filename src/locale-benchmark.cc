// A locale-aware integer formatting benchmark.
//
// Copyright (c) 2019 - present, Victor Zverovich
// All rights reserved.

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <locale>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

#include <fmt/format.h>
#include <benchmark/benchmark.h>

struct separate_thousands : std::numpunct<char> {
  char do_thousands_sep() const { return ','; }
  std::string do_grouping() const { return "\3"; }
};

struct Data {
  std::vector<int> values;
  size_t total_length;

  auto begin() const { return values.begin(); }
  auto end() const { return values.end(); }

  // Prints the number of values by digit count, e.g.
  //  1  27263
  //  2 247132
  //  3 450601
  //  4 246986
  //  5  25188
  //  6   2537
  //  7    251
  //  8     39
  //  9      2
  // 10      1
  void print_digit_counts() const {
    int counts[11] = {};
    for (auto value : values)
      ++counts[fmt::format_int(value).size()];
    fmt::print("The number of values by digit count:\n");
    for (int i = 1; i < 11; ++i)
      fmt::print("{:2} {:6}\n", i, counts[i]);
  }

  Data() : values(1'000'000) {
    // Same data as in Boost Karma int generator test:
    // https://www.boost.org/doc/libs/1_63_0/libs/spirit/workbench/karma/int_generator.cpp
    std::srand(0);
    std::generate(values.begin(), values.end(), []() {
      int scale = std::rand() / 100 + 1;
      return (std::rand() * std::rand()) / scale;
    });
    std::ostringstream os;
    os.imbue(std::locale(std::locale(), new separate_thousands()));
    total_length =
        std::accumulate(begin(), end(), size_t(), [&](size_t lhs, int rhs) {
          os.str(std::string());
          os << rhs;
          return lhs + os.str().size();
        });
    print_digit_counts();
  }
} data;

void finalize(benchmark::State& state, size_t result) {
  auto expected = state.iterations() * data.total_length;
  if (result != expected) {
    throw std::logic_error(
      fmt::format("invalid length: {} != {}", result, expected));
  }
  state.SetItemsProcessed(state.iterations() * data.values.size());
  benchmark::DoNotOptimize(result);
}

void ostringstream(benchmark::State& state) {
  size_t result = 0;
  std::ostringstream os;
  os.imbue(std::locale(std::locale(), new separate_thousands()));
  while (state.KeepRunning()) {
    for (auto value : data) {
      os.str(std::string());
      os << value;
      result += os.str().size();
    }
  }
  finalize(state, result);
}
BENCHMARK(ostringstream);

void format_locale(benchmark::State& state) {
  size_t result = 0;
  auto loc = std::locale(std::locale(), new separate_thousands());
  while (state.KeepRunning()) {
    for (auto value : data) result += fmt::format(loc, "{:L}", value).size();
  }
  finalize(state, result);
}
BENCHMARK(format_locale);

BENCHMARK_MAIN();
