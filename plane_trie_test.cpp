#include "plane_trie.hpp"

#include <stdint.h>

#include <sstream>
#include <vector>
#include <queue>
#include <string>
#include <utility>

#include <iostream>

int main() {


  std::queue< std::pair<std::string, uint32_t> > word_list;

  word_list.push(std::make_pair("a",1));
  word_list.push(std::make_pair("aaaaa",2));
  word_list.push(std::make_pair("aab",3));
  word_list.push(std::make_pair("aaba",4));
  word_list.push(std::make_pair("aabb",5));

  std::stringstream ss;
  build_trie(ss, word_list);

  const char* data = ss.str().data();
  size_t size = ss.str().size();

  trie_t trie;
  load_trie((void*)data, size, trie);

  std::cout << get_node(trie, "a") << std::endl;
  std::cout << get_node(trie, "aa") << std::endl;
  std::cout << get_node(trie, "aaa") << std::endl;
  std::cout << get_node(trie, "aaaaa") << std::endl;
  std::cout << get_node(trie, "aab") << std::endl;

  return 0;

}
