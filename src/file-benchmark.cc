#include <benchmark/benchmark.h>
#include <fmt/compile.h>
#include <fmt/os.h>

#include <fstream>
#include <stdio.h>

auto test_data = "test data";
auto num_iters = 1'000'000;

const char* removed(benchmark::State& state, const char* path) {
  state.PauseTiming();
  std::remove(path);
  state.ResumeTiming();
  return path;
}

void fprintf(benchmark::State& state) {
  for (auto s : state) {
    auto f = fopen(removed(state, "/tmp/fprintf-test"), "wb");
    for (int i = 0; i < num_iters; ++i)
      fprintf(f, "%d\n", test_data);
    fclose(f);
  }
}
BENCHMARK(fprintf);

void std_ofstream(benchmark::State& state) {
  for (auto s : state) {
    auto os = std::ofstream(
      removed(state, "/tmp/ofstream-test"), std::ios::binary);
    for (int i = 0; i < num_iters; ++i)
      os << test_data << '\n';
  }
}
BENCHMARK(std_ofstream);

void fmt_print_compile(benchmark::State& state) {
  for (auto s : state) {
    auto f = fmt::output_file(removed(state, "/tmp/fmt-compile-test"),
                              fmt::buffer_size=state.range(0));
    for (int i = 0; i < num_iters; ++i)
      f.print(FMT_COMPILE("{}\n"), test_data);
  }
}
BENCHMARK(fmt_print_compile)->RangeMultiplier(2)->Range(BUFSIZ, 1 << 20);

void fmt_print_runtime(benchmark::State& state) {
  for (auto s : state) {
    auto f = fmt::output_file(removed(state, "/tmp/fmt-runtime-test"),
                              fmt::buffer_size=state.range(0));
    for (int i = 0; i < num_iters; ++i)
      f.print("{}\n", test_data);
  }
}
BENCHMARK(fmt_print_runtime)->RangeMultiplier(2)->Range(BUFSIZ, 1 << 20);

void fmt_print_compile_default(benchmark::State& state) {
  for (auto s : state) {
    auto f = fmt::output_file(
      removed(state, "/tmp/fmt-compile-default-test"));
    for (int i = 0; i < num_iters; ++i)
      f.print(FMT_COMPILE("{}\n"), test_data);
  }
}
BENCHMARK(fmt_print_compile_default);

BENCHMARK_MAIN();
