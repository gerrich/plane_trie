#include <iostream>
#include "istream_adapter.h"
#include "plane_trie.hpp"

int main() {
  istream_adapter_t adapter(std::cin);
  build_trie(std::cout, adapter);

  return 0;
}
