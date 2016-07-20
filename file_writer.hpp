#pragma once

#include <cstdio>
#include <iostream>

inline size_t _write(FILE* file, void* data, size_t size) {
  return fwrite(data, 1, size, file);
}
inline size_t _write(std::ostream &file, void* data, size_t size) {
  file.write(reinterpret_cast<const char *>(data), size);
  return size;
}

template <typename file_t>
size_t save_to_file(file_t &file, size_t offset, temp_node_t *node) {
//  if (!node) return 0;
  static char zero_str[] = "\0\0\0\0""\0\0\0\0";
  size_t write_count = 0;
  
  size_t child_count = node->key_list.size();
  plane_node_t plane_node = {node->value, child_count};

  write_count += _write(file, &plane_node, round_up_8(sizeof(plane_node)));
  write_count += _write(file, &node->key_list[0], child_count);
  write_count += _write(file, &zero_str[0], round_up_8(child_count) - child_count);
  write_count += _write(file, &node->offset_list[0], round_up_8(sizeof(void*) * child_count));

  return write_count;  
}

class PlaneTrieFileSaver {
public:
  PlaneTrieFileSaver() : offset(0) {}

  template <typename file_t>
  void save(file_t &file, temp_node_t &node) {
    offset += save_to_file(file, offset, node);
  }

public:
  size_t offset; //context
};
