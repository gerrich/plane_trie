#include <gtest/gtest.h>
#include "queue.hpp"

TEST(queue, simple) {
  queue_t<int> q;

  EXPECT_TRUE(q.empty());

  q.push(13);
  EXPECT_FALSE(q.empty());
  EXPECT_EQ(13, q.front());
  EXPECT_EQ(13, q.back());

  q.push(12);
  EXPECT_FALSE(q.empty());
  EXPECT_EQ(13, q.front());
  EXPECT_EQ(12, q.back());


}

TEST(queue, overload) {
  queue_t<int, 3> q;

  int i = 0;
  int j = 0;
  for (; i < 17; ++i) { q.push(i); }
  EXPECT_EQ(0, q.front());
  for (; j < 13; ++j) { q.pop(); }
  EXPECT_EQ(13, q.front());
  for (; i < 34; ++i) { q.push(i); }
  EXPECT_EQ(13, q.front());
  for (; j < 26; ++j) { q.pop(); }
  EXPECT_EQ(26, q.front());
  
  for (; i < 47; ++i) { q.push(i); }
  EXPECT_EQ(26, q.front());
  for (; j < 43; ++j) { q.pop(); }
  EXPECT_EQ(43, q.front());
  for (; i < 60; ++i) { q.push(i); }
  EXPECT_EQ(43, q.front());
  for (; j < 60; ++j) { q.pop(); }
  EXPECT_TRUE(q.empty());
}

TEST(queue, run) {
  queue_t<int, 11> q;

  bool bad = false;
  int j = 0;
  int k = 0;
  for (int i = 0; i < 90; ++i) {
    bool op_type = ((i/9) % 2) == ((i/10) % 2);
    if (op_type) {
      q.push(j++);
    } else {
      if (k != q.front()) {
        bad = true;
        break;
      }
      q.pop();
      ++k;
    }
  } 
  
  EXPECT_FALSE(bad);
  EXPECT_EQ(j, 45);
  EXPECT_EQ(k, 45);
  EXPECT_TRUE(q.empty());

  q.push(j++);
  EXPECT_EQ(45, q.front());
  q.pop();
  ++k;
}
