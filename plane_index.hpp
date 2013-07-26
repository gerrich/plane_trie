#pragma once

#include <assert.h>

struct plane_index_t {

  size_t header_offset_;
  const char *data_;
  size_t size_;
};


void load_index(void *data, size_t size, plane_index_t &plane_index) {
  plane_index.data_ = (const char*)data;
  plane_index.header_offset_ = *(size_t*)&plane_index.data_[size - sizeof(size_t)];
  size_t header_size = size - plane_index.header_offset_;
  assert(header_size % sizeof(size_t) == 0);
  plane_index.size_ = header_size / sizeof(size_t);
}

struct plane_index_item_t {
  size_t size;
  const char *data;
};

plane_index_item_t get_item(const plane_index_t &plane_index, size_t id) {
  size_t item_offset = ((size_t*)(plane_index.data_ + plane_index.header_offset_))[id];
  return (plane_index_item_t){ *(uint16_t*)(&plane_index.data_[item_offset]), &plane_index.data_[item_offset] + 2 };  
}
