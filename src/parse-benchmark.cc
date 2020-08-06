// Format spec parsing benchmark
//
// Copyright (c) 2019 - present, Victor Zverovich
// All rights reserved.

#include <benchmark/benchmark.h>
#include <fmt/format.h>

template <typename Char, typename SpecHandler>
FMT_CONSTEXPR const Char* parse_format_specs_z(const Char* begin,
                                               const Char* end,
                                               SpecHandler&& handler) {
  if (begin == end || *begin == '}') return begin;

  begin = parse_align(begin, end, handler);
  if (begin == end) return begin;

  // Parse sign.
  switch (static_cast<char>(*begin)) {
  case '+':
    handler.on_plus();
    ++begin;
    break;
  case '-':
    handler.on_minus();
    ++begin;
    break;
  case ' ':
    handler.on_space();
    ++begin;
    break;
  }
  if (begin == end) return begin;

  // Parse z flag.
  if (*begin == 'z') {
    handler.on_z();
    if (++begin == end) return begin;
  }

  if (*begin == '#') {
    handler.on_hash();
    if (++begin == end) return begin;
  }

  // Parse zero flag.
  if (*begin == '0') {
    handler.on_zero();
    if (++begin == end) return begin;
  }

  begin = parse_width(begin, end, handler);
  if (begin == end) return begin;

  // Parse precision.
  if (*begin == '.') {
    begin = parse_precision(begin, end, handler);
  }

  // Parse type.
  if (begin != end && *begin != '}') handler.on_type(*begin++);
  return begin;
}

class specs_handler_z
    : public fmt::detail::specs_handler<fmt::format_parse_context,
                                          fmt::format_context> {
 public:
  FMT_CONSTEXPR specs_handler_z(fmt::format_specs& specs,
                                fmt::format_parse_context& parse_ctx,
                                fmt::format_context& ctx)
      : specs_handler(specs, parse_ctx, ctx) {}

  bool z = false;

  void on_z() { z = true; }
};

void parse(benchmark::State& state) {
  auto format_str = fmt::string_view("g");
  auto parse_ctx = fmt::format_parse_context(format_str);
  auto buf = fmt::memory_buffer();
  auto arg_store = fmt::make_format_args(42.0);
  auto args = fmt::format_args(arg_store);
  auto ctx = fmt::format_context(
      std::back_inserter(static_cast<fmt::detail::buffer<char>&>(buf)), args);
  auto specs = fmt::basic_format_specs<char>();
  auto handler = fmt::detail::specs_handler(specs, parse_ctx, ctx);
  while (state.KeepRunning()) {
    fmt::detail::parse_format_specs(format_str.begin(), format_str.end(),
                                      handler);
    benchmark::DoNotOptimize(specs.type);
  }
}
BENCHMARK(parse);

void parse_z(benchmark::State& state) {
  auto format_str = fmt::string_view("g");
  auto parse_ctx = fmt::format_parse_context(format_str);
  auto buf = fmt::memory_buffer();
  auto arg_store = fmt::make_format_args(42.0);
  auto args = fmt::format_args(arg_store);
  auto ctx = fmt::format_context(
      std::back_inserter(static_cast<fmt::detail::buffer<char>&>(buf)), args);
  auto specs = fmt::basic_format_specs<char>();
  auto handler = specs_handler_z(specs, parse_ctx, ctx);
  while (state.KeepRunning()) {
    parse_format_specs_z(format_str.begin(), format_str.end(), handler);
    benchmark::DoNotOptimize(specs.type);
    benchmark::DoNotOptimize(handler.z);
  }
}
BENCHMARK(parse_z);

BENCHMARK_MAIN();
