#include <gtest/gtest.h>
#include <string>
#include <algorithm>

inline bool less(const std::string& s1, const std::string& s2) {
  static std::less<std::string> _less;
  return _less.operator()(s1,s2);
}
TEST(string_less, std_less) {
  EXPECT_TRUE( less("","a"));
  EXPECT_FALSE(less("a","a"));
  EXPECT_FALSE(less("a",""));
  
  EXPECT_TRUE( less("a","aa"));
  EXPECT_FALSE(less("aa","aa"));
  EXPECT_FALSE(less("aa","aa"));
}
