#pragma once

#include "types.hpp"
#include <vector>

struct plane_node_t {
  value_t value;
  uint32_t child_count;
};

struct temp_node_t {
  temp_node_t() : value(0) {}
  value_t value;

  std::vector<char_t> key_list;
  std::vector<temp_node_t*> ptr_list;
  std::vector<size_t> offset_list;
};

/*
size_t calc_node_size(const temp_node_t *node) {
  return sizeof(plane_node_t) + node->offset_list.size() * sizeof(void*) + round_up_8(node->key_list.size());
}
*/
