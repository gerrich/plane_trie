#include <cstdio>
#include "file_adapter.h"
#include <stdint.h>

int main () {
  
  char data_file_tmpl[] = "/tmp/SPC_build_index_XXXXXX";
  char *data_file_name = tmpnam(data_file_tmpl);
  if (!data_file_tmpl) {
    fprintf(stderr, "failed to make tmp file\n");
    return 1;
  }
  FILE *data_file = fopen(data_file_name, "wb+");
  if (!data_file) {
    fprintf(stderr, "failed to open file for writing: %s\n", data_file_name);
    return 1;
  }

  file_adapter_t adapter(stdin);
  size_t offset = 0;
  size_t header_offset = 0;
  while (!adapter.empty()) {
    if (offset % 4 != 0) {
      const char zeroes[] = "\0\0\0\0";
      fwrite(zeroes, 1, 4 - offset%4, stdout);
      offset += 4 - offset%4; // 4 byte alignment
    }

    file_adapter_t::value_type pair = adapter.front();

    uint16_t word_length = pair.first.size();
    fwrite(&offset, sizeof(offset), 1, data_file);
    header_offset += sizeof(offset);

    fwrite(&word_length, 1, sizeof(word_length), stdout);
    fwrite(pair.first.data(), 1, word_length, stdout);
    offset += word_length + sizeof(word_length);

    adapter.pop(); 
  }
  if (offset % 8 != 0) {
    const char zeroes[] = "\0\0\0\0";
    fwrite(zeroes, 1, 8 - offset%8, stdout);
    offset += 8 - offset%8; // 4 byte alignment
  }

  size_t index_begin_offset = offset - sizeof(size_t); // substract (id==0)

  fseek(data_file, 0, SEEK_SET);

  size_t chunk_size = 4*1024*1024;
  char *chunk = new char[chunk_size];
  for (size_t pos = 0; pos < header_offset; pos += chunk_size) {
    size_t read_size = fread(chunk, 1, chunk_size, data_file);
    fwrite(chunk, 1, read_size, stdout);
  }

  fwrite(&index_begin_offset, sizeof(index_begin_offset), 1, stdout);

  fclose(data_file);
  remove(data_file_name);

  return 0;
}
