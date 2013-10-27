//#include <iostream>
//#include "istream_adapter.h"
#include <cstdio>
#include "file_adapter.h"
#include "plane_trie_build.hpp"
#include <cstdio>

int main() {
//  std::ios_base::sync_with_stdio(false);

  file_adapter_t adapter(stdin);
  build_trie(stdout, adapter);

  return 0;
}
