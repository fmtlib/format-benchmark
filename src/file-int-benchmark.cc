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

#if __cpp_lib_concepts>=201907L
#include "../fast_io/include/fast_io.h"
#include "../fast_io/include/fast_io_device.h"
#endif

#include "itostr.cc"

// The method by StackOverflow user
// https://stackoverflow.com/users/2985907/user2985907 sometimes incorrectly
// attributed to jiaendu: https://stackoverflow.com/a/19944488/471164
inline int u2985907_utoa10(unsigned int value, char* str) {
#define JOIN(N) \
  N "0", N "1", N "2", N "3", N "4", N "5", N "6", N "7", N "8", N "9"

#define JOIN2(N)                                                   \
  JOIN(N "0"), JOIN(N "1"), JOIN(N "2"), JOIN(N "3"), JOIN(N "4"), \
      JOIN(N "5"), JOIN(N "6"), JOIN(N "7"), JOIN(N "8"), JOIN(N "9")

#define JOIN3(N)                                                        \
  JOIN2(N "0"), JOIN2(N "1"), JOIN2(N "2"), JOIN2(N "3"), JOIN2(N "4"), \
      JOIN2(N "5"), JOIN2(N "6"), JOIN2(N "7"), JOIN2(N "8"), JOIN2(N "9")

#define JOIN4                                                             \
  JOIN3("0"), JOIN3("1"), JOIN3("2"), JOIN3("3"), JOIN3("4"), JOIN3("5"), \
      JOIN3("6"), JOIN3("7"), JOIN3("8"), JOIN3("9")

#define JOIN5(N)                                                            \
  JOIN(N), JOIN(N "1"), JOIN(N "2"), JOIN(N "3"), JOIN(N "4"), JOIN(N "5"), \
      JOIN(N "6"), JOIN(N "7"), JOIN(N "8"), JOIN(N "9")

#define JOIN6                                                            \
  JOIN5(""), JOIN2("1"), JOIN2("2"), JOIN2("3"), JOIN2("4"), JOIN2("5"), \
      JOIN2("6"), JOIN2("7"), JOIN2("8"), JOIN2("9")

#define F(N) ((N) >= 100 ? 3 : (N) >= 10 ? 2 : 1)

#define F10(N)                                                                \
  F(N), F(N + 1), F(N + 2), F(N + 3), F(N + 4), F(N + 5), F(N + 6), F(N + 7), \
      F(N + 8), F(N + 9)

#define F100(N)                                                            \
  F10(N), F10(N + 10), F10(N + 20), F10(N + 30), F10(N + 40), F10(N + 50), \
      F10(N + 60), F10(N + 70), F10(N + 80), F10(N + 90)

  static const short offsets[] = {F100(0),   F100(100), F100(200), F100(300),
                                  F100(400), F100(500), F100(600), F100(700),
                                  F100(800), F100(900)};

  static const char table1[][4] = {JOIN("")};
  static const char table2[][4] = {JOIN2("")};
  static const char table3[][4] = {JOIN3("")};
  static const char table4[][8] = {JOIN4};
  static const char table5[][4] = {JOIN6};

#undef JOIN
#undef JOIN2
#undef JOIN3
#undef JOIN4
#undef F
#undef F10
#undef F100

  char* wstr;
#if (_WIN64 || __x86_64__ || __ppc64__)
  uint64_t remains[2];
#else
  uint32_t remains[2];
#endif
  unsigned int v2;

  if (value >= 100000000) {
#if (_WIN64 || __x86_64__ || __ppc64__)
    remains[0] = (((uint64_t)value * (uint64_t)3518437209) >> 45);
    remains[1] = (((uint64_t)value * (uint64_t)2882303762) >> 58);
#else
    remains[0] = value / 10000;
    remains[1] = value / 100000000;
#endif
    v2 = remains[1];
    remains[1] = remains[0] - remains[1] * 10000;
    remains[0] = value - remains[0] * 10000;
    if (v2 >= 10) {
      memcpy(str, table5[v2], 2);
      str += 2;
      memcpy(str, table4[remains[1]], 4);
      str += 4;
      memcpy(str, table4[remains[0]], 4);
      return 10;
    } else {
      *(char*)str = v2 + '0';
      str += 1;
      memcpy(str, table4[remains[1]], 4);
      str += 4;
      memcpy(str, table4[remains[0]], 4);
      return 9;
    }
  } else if (value >= 10000) {
#if (_WIN64 || __x86_64__ || __ppc64__)
    v2 = (((uint64_t)value * (uint64_t)3518437209) >> 45);
#else
    v2 = value / 10000;
#endif
    remains[0] = value - v2 * 10000;
    if (v2 >= 1000) {
      memcpy(str, table4[v2], 4);
      str += 4;
      memcpy(str, table4[remains[0]], 4);
      return 8;
    } else {
      wstr = str;
      memcpy(wstr, table5[v2], 4);
      wstr += offsets[v2];
      memcpy(wstr, table4[remains[0]], 4);
      wstr += 4;
      return (wstr - str);
    }
  } else {
    if (value >= 1000) {
      memcpy(str, table4[value], 4);
      return 4;
    } else if (value >= 100) {
      memcpy(str, table3[value], 3);
      return 3;
    } else if (value >= 10) {
      memcpy(str, table2[value], 2);
      return 2;
    } else {
      *(char*)str = *(char*)table1[value];
      return 1;
    }
  }
}

int u2985907_itoa10(int value, char* str) {
  if (value < 0) { *(str++) = '-'; 
    return u2985907_utoa10(-value, str) + 1; 
  }
  else return u2985907_utoa10(value, str);
}

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

void fprintf(benchmark::State& state) {
  std::unique_ptr<FILE,decltype(fclose)*> fp(std::fopen("fprintf.txt","wb"));
  for (auto s : state) {
    for (auto value : data) {
      std::fprintf(fp.get(), "%d\n", value);
    }
  }
}
BENCHMARK(sprintf);

void std_ofstream(benchmark::State& state) {
  std::ofstream os("ofstream.txt",std::ofstream::binary);
  for (auto s : state) {
    for (auto value : data) {
      os << value <<'\n';
    }
  }
}
BENCHMARK(std_ofstream);

void fmt_print(benchmark::State& state) {
  std::ofstream os("fmt_print.txt",std::ofstream::binary);
  for (auto s : state) {
    for (auto value : data) {
      fmt::print(os,"{}\n", value);
    }
  }
}
BENCHMARK(fmt_print);


#if __cpp_lib_concepts>=201907L

void std_to_chars(benchmark::State& state) {
  fast_io::obuf_file obf("std_to_chars.txt");
  for (auto s : state) {
    for (auto value : data) {
      char buffer[13];
      auto res = std::to_chars(buffer, buffer + sizeof(buffer), value);
      *res.ptr=u8'\n';
      write(obf,buffer,++res.ptr);
    }
  }
}
BENCHMARK(std_to_chars);

void fmt_to_string(benchmark::State& state) {
  fast_io::obuf_file obf("fmt_to_string.txt");
  for (auto s : state) {
    for (auto value : data) {
      print(obf,fmt::to_string(value));
    }
  }
}
BENCHMARK(fmt_to_string);

void fmt_format_runtime(benchmark::State& state) {
  fast_io::obuf_file obf("fmt_format_runtime.txt");
  for (auto s : state) {
    for (auto value : data) {
      print(obf,fmt::format("{}\n", value));
    }
  }
}
BENCHMARK(fmt_format_runtime);

void fmt_format_compile(benchmark::State& state) {
  fast_io::obuf_file obf("fmt_format_runtime.txt");
  for (auto s : state) {
    for (auto value : data) {
      print(obf,fmt::format(FMT_COMPILE("{}\n"), value));
    }
  }
}
BENCHMARK(fmt_format_compile);

void fmt_format_to_runtime(benchmark::State& state) {
  fast_io::obuf_file obf("fmt_format_to_runtime.txt");
  for (auto s : state) {
    for (auto value : data) {
      char buffer[13];
      auto end = fmt::format_to(buffer, "{}\n", value);
      write(obf,buffer,end);
    }
  }
}
BENCHMARK(fmt_format_to_runtime);

void fmt_format_to_compile(benchmark::State& state) {
  fast_io::obuf_file obf("fmt_format_to_compile.txt");
  for (auto s : state) {
    for (auto value : data) {
      char buffer[13];
      constexpr auto f = fmt::compile<int>(FMT_STRING("{}"));
      auto end = fmt::format_to(buffer, f, value);
      *end=u8'\n';
      write(obf,buffer,++end);
    }
  }
}
BENCHMARK(fmt_format_to_compile);

void fmt_format_int(benchmark::State& state) {
  fast_io::obuf_file obf("fmt_format_int.txt");
  for (auto s : state) {
    for (auto value : data) {
      auto f = fmt::format_int(value);
      write(obf,f.data(),f.data()+f.size());
      put(obf,u8'\n');
    }
  }
}
BENCHMARK(fmt_format_int);

void boost_lexical_cast(benchmark::State& state) {
  fast_io::obuf_file obf("boost_lexical_cast.txt");
  for (auto s : state) {
    for (auto value : data) {
      println(obf,boost::lexical_cast<std::string>(value));
    }
  }
}
BENCHMARK(boost_lexical_cast);

void boost_format(benchmark::State& state) {
  fast_io::obuf_file obf("boost_format.txt");
  boost::format fmt("%d\n");
  for (auto s : state) {
    for (auto value : data) {
      print(obf,boost::str(fmt % value));
    }
  }
}
BENCHMARK(boost_format);

void boost_karma_generate(benchmark::State& state) {
  fast_io::obuf_file obf("boost_karma_generate.txt");
  for (auto s : state) {
    for (auto value : data) {
      char buffer[13];
      char* ptr = buffer;
      boost::spirit::karma::generate(ptr, boost::spirit::karma::int_, value);
      *ptr=u8'\n';
      write(obf,buffer,++ptr);
    }
  }
}
BENCHMARK(boost_karma_generate);

void voigt_itostr(benchmark::State& state) {
  fast_io::obuf_file obf("voigt_itostr.txt");
  for (auto s : state) {
    for (auto value : data) {
      println(obf,itostr(value));
    }
  }
}
BENCHMARK(voigt_itostr);

void u2985907(benchmark::State& state) {
  fast_io::obuf_file obf("u2985907_itoa10.txt");
  for (auto s : state) {
    for (auto value : data) {
      char buffer[13];
      unsigned size = u2985907_itoa10(value, buffer);
      println(obf,buffer,buffer+size)
    }
  }
}
BENCHMARK(u2985907);

void decimal_from(benchmark::State& state) {
  fast_io::obuf_file obf("decimal_from.txt");
  for (auto s : state) {
    for (auto value : data) {
      char buffer[13];
      auto end = cppx::decimal_from(value, buffer);
      *end=u8'\n';
      write(obf,buffer,++end);
    }
  }
}
BENCHMARK(decimal_from);

void stout_ltoa(benchmark::State& state) {
  fast_io::obuf_file obf("stout_ltoa.txt");
  for (auto s : state) {
    for (auto value : data) {
      char buffer[13];
      ltoa(value, buffer, 13);
      print(obf,fast_io::chvw(buffer));
      // ltoa doesn't give the size so this invokes strlen.
    }
  }
}
BENCHMARK(stout_ltoa);

void fast_io_concat(benchmark::State& state) {
  fast_io::obuf_file obf("fast_io_concatln.txt");
  for (auto s : state) {
    for (auto value : data) {
      println(obf,fast_io::concat(value));
    }
  }
}
BENCHMARK(fast_io_concatln);

void fast_io_concatln(benchmark::State& state) {
  fast_io::obuf_file obf("fast_io_concatln.txt");
  for (auto s : state) {
    for (auto value : data) {
      print(obf,fast_io::concatln(value));
    }
  }
}
BENCHMARK(fast_io_concatln);
void fast_io_print_reserve(benchmark::State& state) {
  fast_io::obuf_file obf("fast_io_print_reserve.txt");
  for (auto s : state) {
    for (auto value : data) {
      auto rsv(fast_io::print_reserve(value));
      println(obf,rsv);
    }
  }
}
BENCHMARK(fast_io_print_reserve);

void fast_io_println(benchmark::State& state) {
  fast_io::obuf_file obf("fast_io_println.txt");
  for (auto s : state) {
    for (auto value : data) {
      println(obf,rsv);
    }
  }
}
BENCHMARK(fast_io_println);
#endif

BENCHMARK_MAIN();
