#include <benchmark/benchmark.h>
#include <algorithm>
#include <random>
#include <cmath>

auto generate_random_data() {
  std::random_device rd;
  std::mt19937 rng(rd());
  std::uniform_int_distribution<int> uni(-300, 300);
  std::vector<int> v;
  std::generate_n(std::back_inserter(v), 100'000, [&] { return uni(rng); });
  return v;
}

auto data = generate_random_data();

void find_pow10_ceil(benchmark::State &s) {
  size_t result = 0;
  while (s.KeepRunning()) {
    for (auto i: data) {
      const double one_over_log2_10 = 0.30102999566398114;  // 1 / log2(10)
      int index = static_cast<int>(
        std::ceil((i + 64 - 1) * one_over_log2_10));
      result += index;
    }
  }
  benchmark::DoNotOptimize(result);
}
BENCHMARK(find_pow10_ceil);

void find_pow10_int(benchmark::State &s) {
  size_t result = 0;
  while (s.KeepRunning()) {
    for (auto i: data) {
        constexpr std::uint64_t log10_2_up_to_32 = 0x4d104d42;
        auto index = int(
            // For arithmetic-shift
            std::int64_t(
            // Calculate 0x0.4d104d42 * exp * 2^32
            std::uint64_t(i + 64 - 1) * log10_2_up_to_32
            // To perform ceiling
            + ((std::uint64_t(1) << 32) - 1)
            ) >> 32
        );
      result += index;
    }
  }
  benchmark::DoNotOptimize(result);
}
BENCHMARK(find_pow10_int);

BENCHMARK_MAIN();
