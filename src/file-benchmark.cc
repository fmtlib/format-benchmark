#include <benchmark/benchmark.h>
#include <fmt/compile.h>
#include <fmt/os.h>

#include <fstream>
#include <stdio.h>

const char* test_str = "Hello, world!";
int num_iters = 1'000'000;

void fprintf(benchmark::State& state) {
  FILE* f = fopen("1.txt", "wb");
  for (auto s : state) {
    for (int i = 0; i < num_iters; ++i)
      fprintf(f, "%s\n", test_str);
  }
  fclose(f);
}
BENCHMARK(fprintf);

void std_ofstream(benchmark::State& state) {
  std::ofstream os("2.txt", std::ios::binary);
  for (auto s : state) {
    for (int i = 0; i < num_iters; ++i)
      os << test_str << '\n';
  }
}
BENCHMARK(std_ofstream);

void fmt_print_compile(benchmark::State& state) {
  auto f = fmt::output_file("3.txt");
  for (auto s : state) {
    for (int i = 0; i < num_iters; ++i)
      f.print(FMT_COMPILE("{}\n"), test_str);
  }
}
BENCHMARK(fmt_print_compile);

void fmt_print_runtime(benchmark::State& state) {
  auto f = fmt::output_file("4.txt");
  for (auto s : state) {
    for (int i = 0; i < num_iters; ++i)
      f.print("{}\n", test_str);
  }
}
BENCHMARK(fmt_print_runtime);

BENCHMARK_MAIN();
