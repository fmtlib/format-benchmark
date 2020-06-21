#include <benchmark/benchmark.h>
#include <fmt/compile.h>

#if __cpp_lib_concepts>=201907L
#include "../fast_io/include/fast_io.h"
#endif

#include <string>

std::string str1 = "label";
std::string str2 = "data1";
std::string str3 = "data2";
std::string str4 = "data3";
std::string str5 = "delim";

void naive(benchmark::State& state) {
  benchmark::ClobberMemory();
  for (auto _ : state) {
    std::string output = "Result: " + str1 + ": (" + str2 + ',' + str3 + ',' +
                         str4 + ',' + str5 + ')';
    benchmark::DoNotOptimize(output.data());
  }
}
BENCHMARK(naive);

void append(benchmark::State& state) {
  benchmark::ClobberMemory();
  for (auto _ : state) {
    std::string output = "Result: ";
    output += str1;
    output += ": (";
    output += str2;
    output += ',';
    output += str3;
    output += ',';
    output += str4;
    output += ',';
    output += str5;
    output += ')';
    benchmark::DoNotOptimize(output.data());
  }
}
BENCHMARK(append);

void appendWithReserve(benchmark::State& state) {
  benchmark::ClobberMemory();
  for (auto _ : state) {
    std::string output = "Result: ";
    output.reserve(str1.length() + str2.length() + str3.length() +
                   str4.length() + str5.length() + 16);
    output += str1;
    output += ": (";
    output += str2;
    output += ',';
    output += str3;
    output += ',';
    output += str4;
    output += ',';
    output += str5;
    output += ')';
    benchmark::DoNotOptimize(output.data());
  }
}
BENCHMARK(appendWithReserve);

void format_compile(benchmark::State& state) {
  benchmark::ClobberMemory();
  for (auto _ : state) {
    auto output = fmt::format(FMT_COMPILE("Result: {}: ({},{},{},{})"), str1,
                              str2, str3, str4, str5);
    benchmark::DoNotOptimize(output.data());
  }
}
BENCHMARK(format_compile);

void format_runtime(benchmark::State& state) {
  benchmark::ClobberMemory();
  for (auto _ : state) {
    auto output =
        fmt::format("Result: {}: ({},{},{},{})", str1, str2, str3, str4, str5);
    benchmark::DoNotOptimize(output.data());
  }
}
BENCHMARK(format_runtime);

void format_to(benchmark::State& state) {
  benchmark::ClobberMemory();
  for (auto _ : state) {
    fmt::memory_buffer output;
    fmt::format_to(output, "Result: {}: ({},{},{},{})", str1, str2, str3, str4,
                   str5);
    benchmark::DoNotOptimize(output.data());
  }
}
BENCHMARK(format_to);
#if __cpp_lib_concepts>=201907L
void fast_io_print(benchmark::State& state) {
  benchmark::ClobberMemory();
  for (auto _ : state) {
    fast_io::internal_temporary_buffer<char> output;
    print(output, "Result: ",str1,": (",str2,",",str3,",",str4,","str5,")");
    benchmark::DoNotOptimize(output.data());
  }
}
BENCHMARK(fast_io_print);
void fast_io_concat(benchmark::State& state) {
  benchmark::ClobberMemory();
  for (auto _ : state) {
    benchmark::DoNotOptimize(fast_io::concat("Result: ",str1,": (",str2,",",str3,",",str4,","str5,")").data());
  }
}
BENCHMARK(fast_io_concat);

#endif

void nullop(benchmark::State& state) {
  for (auto _ : state) {
    benchmark::ClobberMemory();
  }
}
BENCHMARK(nullop);

BENCHMARK_MAIN();
