//#include <iostream>
//#include "istream_adapter.h"
#include <cstdio>
#include "file_adapter.h"
#include "half_trie.hpp"
#include "half_trie_build.hpp"
#include <cstdio>

int main() {
//  std::ios_base::sync_with_stdio(false);

  file_adapter_t adapter(stdin);
  build_half_trie<FILE*, file_adapter_t>(stdout, adapter);

  return 0;
}
