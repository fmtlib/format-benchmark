// A decimal integer to string conversion benchmark
//
// Copyright (c) 2019 - present, Victor Zverovich
// All rights reserved.

#include <benchmark/benchmark.h>
#include <fmt/compile.h>

#include <algorithm>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/spirit/include/karma.hpp>
#include <cstdio>
#include <cstdlib>
#include <limits>
#include <numeric>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#include "itostr.cc"

// Integer to string converter by Alf P. Steinbach modified to return a pointer
// past the end of the output to avoid calling strlen.
namespace cppx {
inline auto unsigned_to_decimal(unsigned long number, char* buffer) {
  if (number == 0) {
    *buffer++ = '0';
  } else {
    char* p_first = buffer;
    while (number != 0) {
      *buffer++ = '0' + number % 10;
      number /= 10;
    }
    std::reverse(p_first, buffer);
  }
  *buffer = '\0';
  return buffer;
}

inline auto to_decimal(long number, char* buffer) {
  if (number < 0) {
    buffer[0] = '-';
    return unsigned_to_decimal(-number, buffer + 1);
  } else {
    return unsigned_to_decimal(number, buffer);
  }
}

inline auto decimal_from(long number, char* buffer) {
  return to_decimal(number, buffer);
}
}  // namespace cppx

// Public domain ltoa by Robert B. Stout dba MicroFirm.
char* ltoa(long N, char* str, int base) {
  int i = 2;
  long uarg;
  constexpr auto BUFSIZE = (sizeof(long) * 8 + 1);
  char *tail, *head = str, buf[BUFSIZE];

  if (36 < base || 2 > base) base = 10; /* can only use 0-9, A-Z        */
  tail = &buf[BUFSIZE - 1];             /* last character position      */
  *tail-- = '\0';

  if (10 == base && N < 0L) {
    *head++ = '-';
    uarg = -N;
  } else
    uarg = N;

  if (uarg) {
    for (i = 1; uarg; ++i) {
      ldiv_t r;

      r = ldiv(uarg, base);
      *tail-- = (char)(r.rem + ((9L < r.rem) ? ('A' - 10L) : '0'));
      uarg = r.quot;
    }
  } else
    *tail-- = '0';

  memcpy(head, ++tail, i);
  return str;
}

struct Data {
  std::vector<int> values;
  size_t total_length;

  auto begin() const { return values.begin(); }
  auto end() const { return values.end(); }

  // Prints the number of values by digit count, e.g.
  //  1  27263
  //  2 247132
  //  3 450601
  //  4 246986
  //  5  25188
  //  6   2537
  //  7    251
  //  8     39
  //  9      2
  // 10      1
  void print_digit_counts() const {
    int counts[11] = {};
    for (auto value : values) ++counts[fmt::format_int(value).size()];
    fmt::print("The number of values by digit count:\n");
    for (int i = 1; i < 11; ++i) fmt::print("{:2} {:6}\n", i, counts[i]);
  }

  Data() : values(1'000'000) {
    // Similar data as in Boost Karma int generator test:
    // https://www.boost.org/doc/libs/1_63_0/libs/spirit/workbench/karma/int_generator.cpp
    // with rand replaced by uniform_int_distribution for consistent results
    // across platforms.
    std::mt19937 gen;
    std::uniform_int_distribution<unsigned> dist(
        0, (std::numeric_limits<int>::max)());
    std::generate(values.begin(), values.end(), [&]() {
      int scale = dist(gen) / 100 + 1;
      return static_cast<int>(dist(gen) * dist(gen)) / scale;
    });
    total_length =
        std::accumulate(begin(), end(), size_t(), [](size_t lhs, int rhs) {
          char buffer[12];
          return lhs + std::sprintf(buffer, "%d", rhs);
        });
    print_digit_counts();
  }
} data;

void finalize(benchmark::State& state, size_t result) {
  if (result != state.iterations() * data.total_length)
    throw std::logic_error("invalid length");
  state.SetItemsProcessed(state.iterations() * data.values.size());
  benchmark::DoNotOptimize(result);
}

void sprintf(benchmark::State& state) {
  size_t result = 0;
  while (state.KeepRunning()) {
    for (auto value : data) {
      char buffer[12];
      result += std::sprintf(buffer, "%d", value);
    }
  }
  finalize(state, result);
}
BENCHMARK(sprintf);

void ostringstream(benchmark::State& state) {
  size_t result = 0;
  std::ostringstream os;
  while (state.KeepRunning()) {
    for (auto value : data) {
      os.str(std::string());
      os << value;
      result += os.str().size();
    }
  }
  finalize(state, result);
}
BENCHMARK(ostringstream);

void to_string(benchmark::State& state) {
  size_t result = 0;
  while (state.KeepRunning()) {
    for (auto value : data) result += std::to_string(value).size();
  }
  finalize(state, result);
}
BENCHMARK(to_string);

void format(benchmark::State& state) {
  size_t result = 0;
  while (state.KeepRunning()) {
    for (auto value : data) result += fmt::format("{}", value).size();
  }
  finalize(state, result);
}
BENCHMARK(format);

void format_compile(benchmark::State& state) {
  size_t result = 0;
  while (state.KeepRunning()) {
    for (auto value : data)
      result += fmt::format(FMT_COMPILE("{}"), value).size();
  }
  finalize(state, result);
}
BENCHMARK(format_compile);

void format_to(benchmark::State& state) {
  size_t result = 0;
  while (state.KeepRunning()) {
    for (auto value : data) {
      char buffer[12];
      result += fmt::format_to(buffer, "{}", value) - buffer;
    }
  }
  finalize(state, result);
}
BENCHMARK(format_to);

void format_to_compile(benchmark::State& state) {
  size_t result = 0;
  while (state.KeepRunning()) {
    for (auto value : data) {
      char buffer[12];
      constexpr auto f = fmt::compile<int>(FMT_STRING("{}"));
      result += fmt::format_to(buffer, f, value) - buffer;
    }
  }
  finalize(state, result);
}
BENCHMARK(format_to_compile);

void format_int(benchmark::State& state) {
  size_t result = 0;
  while (state.KeepRunning()) {
    for (auto value : data) result += fmt::format_int(value).size();
  }
  finalize(state, result);
}
BENCHMARK(format_int);

void lexical_cast(benchmark::State& state) {
  size_t result = 0;
  while (state.KeepRunning()) {
    for (auto value : data)
      result += boost::lexical_cast<std::string>(value).size();
  }
  finalize(state, result);
}
BENCHMARK(lexical_cast);

void boost_format(benchmark::State& state) {
  size_t result = 0;
  boost::format fmt("%d");
  while (state.KeepRunning()) {
    for (auto value : data) result += boost::str(fmt % value).size();
  }
  finalize(state, result);
}
BENCHMARK(boost_format);

void boost_karma_generate(benchmark::State& state) {
  size_t result = 0;
  while (state.KeepRunning()) {
    for (auto value : data) {
      char buffer[12];
      char* ptr = buffer;
      boost::spirit::karma::generate(ptr, boost::spirit::karma::int_, value);
      result += ptr - buffer;
    }
  }
  finalize(state, result);
}
BENCHMARK(boost_karma_generate);

void voigt_itostr(benchmark::State& state) {
  size_t result = 0;
  while (state.KeepRunning()) {
    for (auto value : data) result += itostr(value).size();
  }
  finalize(state, result);
}
BENCHMARK(voigt_itostr);

void decimal_from(benchmark::State& state) {
  size_t result = 0;
  while (state.KeepRunning()) {
    for (auto value : data) {
      char buffer[12];
      result += cppx::decimal_from(value, buffer) - buffer;
    }
  }
  benchmark::DoNotOptimize(result);
  finalize(state, result);
}
BENCHMARK(decimal_from);

void stout_ltoa(benchmark::State& state) {
  size_t result = 0;
  while (state.KeepRunning()) {
    for (auto value : data) {
      char buffer[12];
      // ltoa doesn't give the size so we have to call strlen.
      result += strlen(ltoa(value, buffer, 10));
    }
  }
  finalize(state, result);
}
BENCHMARK(stout_ltoa);

BENCHMARK_MAIN();
