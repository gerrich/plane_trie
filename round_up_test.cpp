#include <gtest/gtest.h>
#include "plane_trie.hpp"

TEST(round_up, round_up_8) {

  EXPECT_EQ(0, round_up_8(0));

  EXPECT_EQ(8, round_up_8(1));
  EXPECT_EQ(8, round_up_8(2));
  EXPECT_EQ(8, round_up_8(3));
  EXPECT_EQ(8, round_up_8(4));
  EXPECT_EQ(8, round_up_8(5));
  EXPECT_EQ(8, round_up_8(6));
  EXPECT_EQ(8, round_up_8(7));
  EXPECT_EQ(8, round_up_8(8));

  EXPECT_EQ(16, round_up_8(0x9));
  EXPECT_EQ(16, round_up_8(0xa));
  EXPECT_EQ(16, round_up_8(0xb));
  EXPECT_EQ(16, round_up_8(0xc));
  EXPECT_EQ(16, round_up_8(0xd));
  EXPECT_EQ(16, round_up_8(0xe));
  EXPECT_EQ(16, round_up_8(0xf));
  EXPECT_EQ(16, round_up_8(0x10));
}
