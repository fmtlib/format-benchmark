#include <benchmark/benchmark.h>
#include <fmt/core.h>

void fmt_format(benchmark::State &state) {
  while (state.KeepRunning())
    fmt::format("{}", 42.0);
}

BENCHMARK(fmt_format);

BENCHMARK_MAIN();
