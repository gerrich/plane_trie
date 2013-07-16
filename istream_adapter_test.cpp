#include "istream_adapter.h"
#include "gtest/gtest.h"

TEST(istream_adapter, simple) {
  std::stringstream ss("a\nc\nd");

  istream_adapter_t adapter(ss);
  
  EXPECT_FALSE(adapter.empty());
  EXPECT_STREQ("a", adapter.front().first.c_str());
  EXPECT_EQ(1, adapter.front().second);

  adapter.pop();
  
  EXPECT_FALSE(adapter.empty());
  EXPECT_STREQ("c", adapter.front().first.c_str());
  EXPECT_EQ(2, adapter.front().second);
  
  adapter.pop();
  
  EXPECT_FALSE(adapter.empty());
  EXPECT_STREQ("d", adapter.front().first.c_str());
  EXPECT_EQ(3, adapter.front().second);
  
  adapter.pop();
  EXPECT_TRUE(adapter.empty());
}
