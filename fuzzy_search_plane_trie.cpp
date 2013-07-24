#include <iostream>
#include <set>
#include <iterator>

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

  size_t max_dist = 3;

  while(std::cin.good()) {
    std::string line;
    getline(std::cin, line);
    if (line.empty()) continue;

    std::set<uint32_t> result;
    fuzzy_search(trie, line.c_str(), max_dist, result);
    
    std::cout << line << "\t";
    std::copy(result.begin(), result.end(), std::ostream_iterator<uint32_t>(std::cout, " "));
    std::cout << std::endl;
    //std::cout << line << "\t" << value << std::endl;
  }

  return 0;
}
