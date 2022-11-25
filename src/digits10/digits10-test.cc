#include "digits10.h"

#include <gmock/gmock.h>

using std::uint32_t;

void test_digits10(uint32_t (*digits10)(uint32_t n)) {
  for (uint32_t i = 0; i < 10; ++i) EXPECT_EQ(1u, digits10(i));
  for (uint32_t i = 1, n = 1, end = std::numeric_limits<uint32_t>::max() / 10;
       n <= end; ++i) {
    n *= 10;
    EXPECT_EQ(i, digits10(n - 1));
    EXPECT_EQ(i + 1, digits10(n));
  }
}

TEST(Digits10Test, Digits10) {
  test_digits10(digits10_naive);
  test_digits10(digits10_unroll4);
  test_digits10(digits10_clz);
  test_digits10(digits10_clz_zverovich);
}

TEST(Digits10Test, MinNumber) {
  EXPECT_EQ(0, min_number(1));
  EXPECT_EQ(10, min_number(2));
  EXPECT_EQ(100, min_number(3));
  EXPECT_EQ(1000000000, min_number(10));
  EXPECT_THROW(min_number(0), std::out_of_range);
  EXPECT_THROW(min_number(11), std::out_of_range);
}

TEST(Digits10Test, MaxNumber) {
  EXPECT_EQ(9, max_number(1));
  EXPECT_EQ(99, max_number(2));
  EXPECT_EQ(999, max_number(3));
  EXPECT_EQ(999999999, max_number(9));
  EXPECT_EQ(std::numeric_limits<std::uint32_t>::max(), max_number(10));
  EXPECT_THROW(max_number(0), std::out_of_range);
  EXPECT_THROW(max_number(11), std::out_of_range);
}

TEST(Digits10Test, GenerateNumbers) {
  const std::size_t size = 100;
  auto n1 = generate_numbers(3);
  auto n2 = generate_numbers(3);
  EXPECT_EQ(size, n1.size());
  for (std::size_t i = 0; i < size; ++i) {
    EXPECT_EQ(3, digits10_naive(n1[i]));
    EXPECT_EQ(n1[i], n2[i]);
  }
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
