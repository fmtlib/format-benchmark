// Benchmark varargs overhead.

#include <cstdarg>
#include "benchmark/benchmark.h"
#include "fmt/format.h"

int __attribute__((noinline)) test_vprintf(const char *f, std::va_list) {
  benchmark::DoNotOptimize(f);
  return 0;
}

int test_printf(const char *format, ...) {
  std::va_list args;
  va_start(args, format);
  int result = test_vprintf(format, args);
  va_end(args);
  return result;
}

void varargs(benchmark::State& state) {
  while (state.KeepRunning())
    test_printf("%d", 42);
}

void varargs_12(benchmark::State& state) {
  while (state.KeepRunning())
    test_printf("%d %d %d %d "
                "%f %f %f %f "
                "%x %x %x %x",
                1,    2,    3,    4,
                1.0f, 2.0f, 3.0f, 4.0f,
                0x1,  0x2,  0x3,  0x4);
}

BENCHMARK(varargs);

void __attribute__((noinline)) test_vprint(const char *f, fmt::format_args) {
  benchmark::DoNotOptimize(f);
}

template <typename ... Args>
inline void test_print(const char *format, const Args & ... args) {
  test_vprint(format, fmt::make_format_args<fmt::BasicFormatter<char>>(args...));
}

void fmt_variadic(benchmark::State &state) {
  while (state.KeepRunning())
    test_print("{}", 42);
}

void fmt_variadic_12(benchmark::State &state) {
  while (state.KeepRunning())
    test_printf("{} {} {} {} "
                "{} {} {} {} "
                "{} {} {} {}",
                1,    2,    3,    4,
                1.0f, 2.0f, 3.0f, 4.0f,
                0x1,  0x2,  0x3,  0x4);
}

BENCHMARK(fmt_variadic);

void test_sprintf(benchmark::State &state) {
  char buffer[64];
  while (state.KeepRunning())
    std::sprintf(buffer, "%d", 42);
}

void test_sprintf_12(benchmark::State &state) {
  char buffer[64];
  while (state.KeepRunning())
    std::sprintf(buffer, "%d %d %d %d "
                         "%f %f %f %f "
                         "%x %x %x %x",
                         1,    2,    3,    4,
                         1.0f, 2.0f, 3.0f, 4.0f,
                         0x1,  0x2,  0x3,  0x4);
}

BENCHMARK(test_sprintf);

void test_format(benchmark::State &state) {
  while (state.KeepRunning())
    fmt::format("{}", 42);
}

void test_format_12(benchmark::State &state) {
  while (state.KeepRunning())
      fmt::format("{} {} {} {} "
                  "{} {} {} {} "
                  "{} {} {} {}",
                  1,    2,    3,    4,
                  1.0f, 2.0f, 3.0f, 4.0f,
                  0x1,  0x2,  0x3,  0x4);
}

BENCHMARK(test_format);

void test_sprintf_pos(benchmark::State &state) {
  char buffer[64];
  while (state.KeepRunning())
    std::sprintf(buffer, "%1$d", 42);
}

void test_sprintf_pos_12(benchmark::State &state) {
  char buffer[64];
  while (state.KeepRunning())
    std::sprintf(buffer, "%12$d %11$d %10$d %9$d "
                         "%8$f %7$f %6$f %5$f "
                         "%4$x %3$x %2$x %1$x",
                         0x1,  0x2,  0x3,  0x4,
                         1.0f, 2.0f, 3.0f, 4.0f,
                         1,    2,    3,    4);
}

BENCHMARK(test_sprintf_pos);

void test_format_pos(benchmark::State &state) {
  while (state.KeepRunning())
    fmt::format("{0}", 42);
}

void test_format_pos_12(benchmark::State &state) {
  while (state.KeepRunning())
      fmt::format("{11} {10} {9} {8} "
                  "{7} {6} {5} {4} "
                  "{3} {2} {1} {0}",
                  1,    2,    3,    4,
                  1.0f, 2.0f, 3.0f, 4.0f,
                  0x1,  0x2,  0x3,  0x4);
}

BENCHMARK(test_format_pos);

BENCHMARK(varargs_12);
BENCHMARK(fmt_variadic_12);
BENCHMARK(test_sprintf_12);
BENCHMARK(test_format_12);
BENCHMARK(test_sprintf_pos_12);
BENCHMARK(test_format_pos_12);

BENCHMARK_MAIN();
