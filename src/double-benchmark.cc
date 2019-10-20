// Benchmark roundtrip double format.

#include "dtoa_milo.h"
#include <benchmark/benchmark.h>
#include <cstdio>
#include <fmt/format.h>
#include <random>
#include <vector>

std::vector<double> generate_random_data() {
  auto data = std::vector<double>();
  auto gen = std::mt19937_64();
  for (auto i = 0; i < 1000; ++i) {
    auto d = fmt::internal::bit_cast<double>(gen());
    data.push_back(d);
    fmt::print("{}\n", d);
  }
  return data;
}

auto data = generate_random_data();

void sprintf(benchmark::State &state) {
  char buf[100];
  while (state.KeepRunning()) {
    for (auto n : data) {
      // Set precision to 17 to satisfy roundtrip guarantees.
      std::sprintf(buf, "%.17g", n);
    }
  }
}

BENCHMARK(sprintf);

void format_to(benchmark::State &state) {
  char buf[100];
  while (state.KeepRunning()) {
    for (auto n : data)
      fmt::format_to(buf, "{}", n);
  }
}

BENCHMARK(format_to);

void dtoa_milo(benchmark::State &state) {
  char buf[100];
  while (state.KeepRunning()) {
    for (auto n : data)
      dtoa_milo(n, buf);
  }
}

BENCHMARK(dtoa_milo);

BENCHMARK_MAIN();
