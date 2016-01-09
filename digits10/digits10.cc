#include "digits10.h"
#include <random>

using std::uint32_t;

const uint32_t powers_of_10_u32[] = {
  0,
  10,
  100,
  1000,
  10000,
  100000,
  1000000,
  10000000,
  100000000,
  1000000000
};

std::vector<uint32_t> generate_numbers(int num_digits) {
  // Use fixed seed to generate identical sequences.
  std::mt19937 gen(0);
  std::uniform_int_distribution<uint32_t>
    dis(min_number(num_digits), max_number(num_digits));
  std::vector<uint32_t> result;
  int count = 100;
  result.reserve(count);
  for (int i = 0; i < count; ++i)
    result.push_back(dis(gen));
  return result;
}
