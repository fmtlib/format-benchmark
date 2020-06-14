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
#include <charconv>
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

// Computes a digest of data. It is used both to prevent compiler from
// optimizing away the benchmarked code and to verify that the results are
// correct. The overhead is less than 2.5% compared to just DoNotOptimize.
FMT_INLINE unsigned compute_digest(fmt::string_view data) {
  unsigned digest = 0;
  for (char c : data) digest += c;
  return digest;
}

struct Data {
  std::vector<int> values;
  unsigned digest;

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
    digest =
        std::accumulate(begin(), end(), unsigned(), [](unsigned lhs, int rhs) {
          char buffer[12];
          unsigned size = std::sprintf(buffer, "%d", rhs);
          return lhs + compute_digest({buffer, size});
        });
    print_digit_counts();
  }
} data;

struct digest_checker {
  benchmark::State& state;
  unsigned digest = 0;

  explicit digest_checker(benchmark::State& s) : state(s) {}

  ~digest_checker() noexcept(false) {
    if (digest != static_cast<unsigned>(state.iterations()) * data.digest)
      throw std::logic_error("invalid length");
    state.SetItemsProcessed(state.iterations() * data.values.size());
    benchmark::DoNotOptimize(digest);
  }

  FMT_INLINE void add(fmt::string_view s) {
    digest += compute_digest(s);
  }
};

void finalize(benchmark::State& state, unsigned result) {
  if (result != static_cast<unsigned>(state.iterations()) * data.digest)
    throw std::logic_error("invalid length");
  state.SetItemsProcessed(state.iterations() * data.values.size());
  benchmark::DoNotOptimize(result);
}

void sprintf(benchmark::State& state) {
  auto dc = digest_checker(state);
  while (state.KeepRunning()) {
    for (auto value : data) {
      char buffer[12];
      unsigned size = std::sprintf(buffer, "%d", value);
      dc.add({buffer, size});
    }
  }
}
BENCHMARK(sprintf);

void ostringstream(benchmark::State& state) {
  auto dc = digest_checker(state);
  std::ostringstream os;
  while (state.KeepRunning()) {
    for (auto value : data) {
      os.str(std::string());
      os << value;
      std::string s = os.str();
      dc.add(s);
    }
  }
}
BENCHMARK(ostringstream);

void std_to_string(benchmark::State& state) {
  auto dc = digest_checker(state);
  while (state.KeepRunning()) {
    for (auto value : data) {
      std::string s = std::to_string(value);
      dc.add(s);
    }
  }
}
BENCHMARK(std_to_string);

void std_to_chars(benchmark::State& state) {
  auto dc = digest_checker(state);
  while (state.KeepRunning()) {
    for (auto value : data) {
      char buffer[12];
      auto res = std::to_chars(buffer, buffer + sizeof(buffer), value);
      unsigned size = res.ptr - buffer;
      dc.add({buffer, size});
    }
  }
}
BENCHMARK(std_to_chars);

void fmt_to_string(benchmark::State& state) {
  auto dc = digest_checker(state);
  while (state.KeepRunning()) {
    for (auto value : data) {
      std::string s = fmt::to_string(value);
      dc.add(s);
    }
  }
}
BENCHMARK(fmt_to_string);

void format_runtime(benchmark::State& state) {
  auto dc = digest_checker(state);
  while (state.KeepRunning()) {
    for (auto value : data) {
      std::string s = fmt::format("{}", value);
      dc.add(s);
    }
  }
}
BENCHMARK(format_runtime);

void format_compile(benchmark::State& state) {
  auto dc = digest_checker(state);
  while (state.KeepRunning()) {
    for (auto value : data) {
      std::string s = fmt::format(FMT_COMPILE("{}"), value);
      dc.add(s);
    }
  }
}
BENCHMARK(format_compile);

void format_to_runtime(benchmark::State& state) {
  auto dc = digest_checker(state);
  while (state.KeepRunning()) {
    for (auto value : data) {
      char buffer[12];
      auto end = fmt::format_to(buffer, "{}", value);
      unsigned size = end - buffer;
      dc.add({buffer, size});
    }
  }
}
BENCHMARK(format_to_runtime);

void format_to_compile(benchmark::State& state) {
  auto dc = digest_checker(state);
  while (state.KeepRunning()) {
    for (auto value : data) {
      char buffer[12];
      constexpr auto f = fmt::compile<int>(FMT_STRING("{}"));
      auto end = fmt::format_to(buffer, f, value);
      unsigned size = end - buffer;
      dc.add({buffer, size});
    }
  }
}
BENCHMARK(format_to_compile);

void format_int(benchmark::State& state) {
  auto dc = digest_checker(state);
  while (state.KeepRunning()) {
    for (auto value : data) {
      auto f = fmt::format_int(value);
      dc.add({f.data(), f.size()});
    }
  }
}
BENCHMARK(format_int);

void lexical_cast(benchmark::State& state) {
  auto dc = digest_checker(state);
  while (state.KeepRunning()) {
    for (auto value : data) {
      std::string s = boost::lexical_cast<std::string>(value);
      dc.add(s);
    }
  }
}
BENCHMARK(lexical_cast);

void boost_format(benchmark::State& state) {
  auto dc = digest_checker(state);
  boost::format fmt("%d");
  while (state.KeepRunning()) {
    for (auto value : data) {
      std::string s = boost::str(fmt % value);
      dc.add(s);
    }
  }
}
BENCHMARK(boost_format);

void boost_karma_generate(benchmark::State& state) {
  auto dc = digest_checker(state);
  while (state.KeepRunning()) {
    for (auto value : data) {
      char buffer[12];
      char* ptr = buffer;
      boost::spirit::karma::generate(ptr, boost::spirit::karma::int_, value);
      unsigned size = ptr - buffer;
      dc.add({buffer, size});
    }
  }
}
BENCHMARK(boost_karma_generate);

void voigt_itostr(benchmark::State& state) {
  auto dc = digest_checker(state);
  while (state.KeepRunning()) {
    for (auto value : data) {
      std::string s = itostr(value);
      dc.add(s);
    }
  }
}
BENCHMARK(voigt_itostr);

void decimal_from(benchmark::State& state) {
  auto dc = digest_checker(state);
  while (state.KeepRunning()) {
    for (auto value : data) {
      char buffer[12];
      auto end = cppx::decimal_from(value, buffer);
      unsigned size = end - buffer;
      dc.add({buffer, size});
    }
  }
}
BENCHMARK(decimal_from);

void stout_ltoa(benchmark::State& state) {
  auto dc = digest_checker(state);
  while (state.KeepRunning()) {
    for (auto value : data) {
      char buffer[12];
      ltoa(value, buffer, 10);
      // ltoa doesn't give the size so this invokes strlen.
      dc.add(buffer);
    }
  }
}
BENCHMARK(stout_ltoa);

BENCHMARK_MAIN();
