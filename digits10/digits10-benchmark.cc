#include "digits10.h"

static void num_digits(benchmark::internal::Benchmark* b) {
  for (int i = 1; i <= 10; ++i)
    b->Arg(i);
}

static void naive(benchmark::State& state) {
  run_benchmark(state, digits10_naive);
}
BENCHMARK(naive)->Apply(num_digits);

static void unroll4(benchmark::State& state) {
  run_benchmark(state, digits10_unroll4);
}
BENCHMARK(unroll4)->Apply(num_digits);

static void clz(benchmark::State& state) {
  run_benchmark(state, digits10_clz);
}
BENCHMARK(clz)->Apply(num_digits);

static void clz_zverovich(benchmark::State& state) {
  run_benchmark(state, digits10_clz_zverovich);
}
BENCHMARK(clz_zverovich)->Apply(num_digits);

BENCHMARK_MAIN();
