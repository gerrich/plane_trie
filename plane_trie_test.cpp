#include "gtest/gtest.h"

#include "plane_trie.hpp"
#include "plane_trie_build.hpp"

#include <stdint.h>

#include <sstream>
#include <vector>
#include <queue>
#include <string>
#include <utility>


TEST(plane_trie, simple_test) {

  std::queue< std::pair<std::string, uint32_t> > word_list;

  word_list.push(std::make_pair("a",1));
  word_list.push(std::make_pair("aaaaa",2));
  word_list.push(std::make_pair("aab",3));
  word_list.push(std::make_pair("aaba",4));
  word_list.push(std::make_pair("aabb",5));

  std::stringstream ss;
  build_trie(ss, word_list);

  std::string content = ss.str();
  const char* data = content.data();
  size_t size = content.size();

  trie_t trie;
  load_trie((void*)data, size, trie);

  EXPECT_EQ(1, get_node(trie, "a"));
  EXPECT_EQ(0, get_node(trie, "aa"));
  EXPECT_EQ(0, get_node(trie, "aaa"));
  EXPECT_EQ(2, get_node(trie, "aaaaa"));
  EXPECT_EQ(3, get_node(trie, "aab"));
  EXPECT_EQ(4, get_node(trie, "aaba"));
  EXPECT_EQ(5, get_node(trie, "aabb"));
  EXPECT_EQ(0, get_node(trie, "abb"));
  EXPECT_EQ(0, get_node(trie, "b"));
}
