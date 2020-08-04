#include <benchmark/benchmark.h>
#include <fmt/compile.h>
#include <fmt/os.h>

#include <fstream>
#include <stdio.h>

auto test_str = "[testdata]";
auto num_iters = 1'000'000;

void fprintf(benchmark::State& state) {
  auto path = "/tmp/fprintf-test.txt";
  auto f = fopen(path, "wb");
  for (auto s : state) {
    for (int i = 0; i < num_iters; ++i)
      fprintf(f, "%s\n", test_str);
  }
  fclose(f);
}
BENCHMARK(fprintf);

void std_ofstream(benchmark::State& state) {
  auto path = "/tmp/ofstream-test.txt";
  std::remove(path);
  auto os = std::ofstream(path, std::ios::binary);
  for (auto s : state) {
    for (int i = 0; i < num_iters; ++i)
      os << test_str << '\n';
  }
}
BENCHMARK(std_ofstream);

void fmt_print_compile(benchmark::State& state) {
  auto path = "/tmp/fmt-compile-test.txt";
  std::remove(path);
  auto f = fmt::output_file(path, fmt::buffer_size=state.range(0));
  for (auto s : state) {
    for (int i = 0; i < num_iters; ++i)
      f.print(FMT_COMPILE("{}\n"), test_str);
  }
}
BENCHMARK(fmt_print_compile)->RangeMultiplier(2)->Range(BUFSIZ, 1 << 20);

void fmt_print_runtime(benchmark::State& state) {
  auto path = "/tmp/fmt-runtime-test.txt";
  std::remove(path);
  auto f = fmt::output_file(path, fmt::buffer_size=state.range(0));
  for (auto s : state) {
    for (int i = 0; i < num_iters; ++i)
      f.print("{}\n", test_str);
  }
}
BENCHMARK(fmt_print_runtime)->RangeMultiplier(2)->Range(BUFSIZ, 1 << 20);

void fmt_print_compile_default(benchmark::State& state) {
  auto path = "/tmp/fmt-compile-default-test.txt";
  std::remove(path);
  auto f = fmt::output_file(path);
  for (auto s : state) {
    for (int i = 0; i < num_iters; ++i)
      f.print(FMT_COMPILE("{}\n"), test_str);
  }
}
BENCHMARK(fmt_print_compile_default);

BENCHMARK_MAIN();
