#include <iostream>
#include <set>
#include <iterator>

#include "plane_trie.hpp"
#include "plane_index.hpp"
#include "mmap.hpp"


int main(int argc, char**argv) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " <max-dist> <plane_trie.bin> [<plane_index.bin>]" << std::endl;
    return 1;
  }
  mmap_t my_map(argv[2]);
 
  trie_t trie;
  load_trie(my_map.data(), my_map.size(), trie);
  
  mmap_t index_map;
  plane_index_t index;
  bool has_index = false;
  if (argc >=4) {
    has_index = true;
    mmap_t tmp_map(argv[3]);
    std::swap(index_map, tmp_map);
    load_index(index_map.data(), index_map.size(), index);
  }


  size_t max_dist = atoi(argv[1]);

  while(std::cin.good()) {
    std::string line;
    getline(std::cin, line);
    if (line.empty()) continue;

    std::set<uint32_t> result;
    inserter_out_t< std::set<uint32_t> > out(result); 
    fuzzy_search(trie, line.c_str(), max_dist, out);
    
    std::cout << line << "\t";
    if (has_index) {
      for (std::set<uint32_t>::const_iterator it = result.begin(); it != result.end(); ++it) {
        if (it != result.begin()) std::cout << " ";
        plane_index_item_t item = get_item(index, *it);
        std:: cout << "[" << item.size << "]";
        std::cout << std::string(item.data, item.size);
      }
    } else {
      std::copy(result.begin(), result.end(), std::ostream_iterator<uint32_t>(std::cout, " "));
    }
    std::cout << std::endl;
    //std::cout << line << "\t" << value << std::endl;
  }

  return 0;
}
