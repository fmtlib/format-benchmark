#include <stdint.h>
#include <fmt/compile.h>
#include "test.h"

void u32toa_fmt(uint32_t value, char* buffer) {
    *fmt::format_to(buffer, FMT_COMPILE("{}"), value) = '\0';
}

void i32toa_fmt(int32_t value, char* buffer) {
    *fmt::format_to(buffer, FMT_COMPILE("{}"), value) = '\0';
}

void u64toa_fmt(uint64_t value, char* buffer) {
    *fmt::format_to(buffer, FMT_COMPILE("{}"), value) = '\0';
}

void i64toa_fmt(int64_t value, char* buffer) {
    *fmt::format_to(buffer, FMT_COMPILE("{}"), value) = '\0';
}

REGISTER_TEST(fmt);
