#include <iostream>
#include "half_trie.hpp"

#include "mmap.hpp"

int main(int argc, char**argv) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <trie.bin>" << std::endl;
    return 1;
  }

  mmap_t my_map(argv[1]);
 
  half_trie_t trie;
  load_half_trie(my_map.data(), my_map.size(), trie);
  
  std::ios_base::sync_with_stdio(false);

  while(std::cin.good()) {
    std::string line;
    getline(std::cin, line);
    if (line.empty()) continue;

    uint32_t value;
    if (find(trie, line.c_str(), value)) {
      std::cout << line << "\t" << value << std::endl;
    } else {
      std::cout << line << "\t" << std::endl;
    }
  }

  return 0;
}
