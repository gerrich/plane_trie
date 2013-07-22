#include <iostream>
#include "plane_trie.hpp"

#include "mmap.hpp"

int main(int argc, char**argv) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <trie.bin>" << std::endl;
    return 1;
  }

  mmap_t my_map(argv[1]);
 
  trie_t trie;
  load_trie(my_map.data(), my_map.size(), trie);
  
  std::ios_base::sync_with_stdio(false);

  while(std::cin.good()) {
    std::string line;
    getline(std::cin, line);
    if (line.empty()) continue;

    uint32_t value = get_node(trie, line.c_str());
    std::cout << line << "\t" << value << std::endl;
  }

  return 0;
}
