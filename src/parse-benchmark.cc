// Format spec parsing benchmark
//
// Copyright (c) 2019 - present, Victor Zverovich
// All rights reserved.

#include <benchmark/benchmark.h>
#include <fmt/core.h>
#include <fmt/format.h>

namespace cur_version = fmt::v9;
namespace cur_detail = cur_version::detail;

template <typename Char, typename SpecHandler>
FMT_CONSTEXPR FMT_INLINE auto parse_format_specs_z(const Char* begin,
                                                 const Char* end,
                                                 SpecHandler&& handler)
    -> const Char* {
  if (1 < end - begin && begin[1] == '}' && cur_detail::is_ascii_letter(*begin) &&
      *begin != 'L') {
    cur_version::presentation_type type = cur_detail::parse_presentation_type(*begin++);
    if (type == cur_version::presentation_type::none)
      handler.on_error("invalid type specifier");
    handler.on_type(type);
    return begin;
  }

  if (begin == end) return begin;

  begin = parse_align(begin, end, handler);
  if (begin == end) return begin;

  // Parse sign.
  switch (cur_detail::to_ascii(*begin)) {
  case '+':
    handler.on_sign(cur_version::sign::plus);
    ++begin;
    break;
  case '-':
    handler.on_sign(cur_version::sign::minus);
    ++begin;
    break;
  case ' ':
    handler.on_sign(cur_version::sign::space);
    ++begin;
    break;
  default:
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
    if (begin == end) return begin;
  }

  if (*begin == 'L') {
    handler.on_localized();
    ++begin;
  }

  // Parse type.
  if (begin != end && *begin != '}') {
    cur_version::presentation_type type = cur_detail::parse_presentation_type(*begin++);
    if (type == cur_version::presentation_type::none)
      handler.on_error("invalid type specifier");
    handler.on_type(type);
  }
  return begin;
}

class specs_handler_z
    : public cur_detail::specs_handler<char> {
 public:
  FMT_CONSTEXPR specs_handler_z(cur_version::format_specs& specs,
                                cur_version::format_parse_context& parse_ctx,
                                cur_version::format_context& ctx)
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
  auto handler = cur_detail::specs_handler<char>(specs, parse_ctx, ctx);
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
