#include <cstdio>
#include "file_adapter.h"

int main() {

  file_adapter_t adapter(stdin);

  while(!adapter.empty()) {
    const char *str = adapter.front().first.c_str();
    if (str[0] == '\0') continue;
    fprintf(stdout, "%s\t%u\n", str, adapter.front().second);
    adapter.pop();
  }

  return 0;
}

