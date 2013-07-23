#include <iostream>
#include <iterator>

#include "plane_trie.hpp"
#include "mmap.hpp"

struct printer_t {
  printer_t():last_pair_(0, 0)  {}

  void insert(const std::pair<uint32_t, uint32_t>& pair) {
    if (last_pair_ == pair) return;
    std::cout << pair.first << "\t" << pair.second << std::endl;
    last_pair_ = pair;
  }
  
  std::pair<uint32_t, uint32_t> last_pair_;
};

int main(int argc, char**argv) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " <dict_trie.bin> <word_trie.bin>" << std::endl;
    return 1;
  }

  mmap_t dict_map(argv[1]);
  trie_t dict_trie;
  load_trie(dict_map.data(), dict_map.size(), dict_trie);
  
  mmap_t word_map(argv[2]);
  trie_t word_trie;
  load_trie(word_map.data(), word_map.size(), word_trie);

  size_t max_dist = 3;

  printer_t printer;
  fuzzy_search(dict_trie, word_trie, NULL, NULL, 0, printer);

  return 0;
}
