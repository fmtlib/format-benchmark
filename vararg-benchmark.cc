// Benchmark varargs overhead.

#include <cstdarg>
#include "benchmark/benchmark.h"
#include "fmt/format.h"

int __attribute__((noinline)) test_vprintf(const char *, std::va_list) {
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

BENCHMARK(varargs);

void __attribute__((noinline)) test_vprint(const char *, fmt::format_args ) {
}

template <typename ... Args>
inline void test_print(const char *format, const Args & ... args) {
  test_vprint(format, fmt::make_format_args<fmt::BasicFormatter<char>>(args...));
}

void fmt_variadic(benchmark::State &state) {
  while (state.KeepRunning())
    test_print("{}", 42);
}

BENCHMARK(fmt_variadic);

void test_sprintf(benchmark::State &state) {
  char buffer[64];
  while (state.KeepRunning())
    std::sprintf(buffer, "%d", 42);
}

BENCHMARK(test_sprintf);

void test_format(benchmark::State &state) {
  while (state.KeepRunning())
    fmt::format("{}", 42);
}

BENCHMARK(test_format);

BENCHMARK_MAIN();
