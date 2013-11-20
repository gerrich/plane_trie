#pragma once

#include "plane_trie.hpp"
#include <cstring>

struct half_trie_t {
  trie_t prefix_trie;

  void *data_;
  size_t size_;
  
  size_t second_offset;
  size_t *offset_list;
  // TODO: second trie storage
};

inline size_t _get_offset_by_id(const half_trie_t &half_trie, uint32_t id) {
  return half_trie.offset_list[id];
}

bool _get_second_trie_by_id(const half_trie_t &half_trie, uint32_t second_trie_id, trie_t &second_trie) {
  load_trie_by_root_offset(
      (void*)((char*)(half_trie.data_) + half_trie.second_offset),
      _get_offset_by_id(half_trie, second_trie_id),
      second_trie
    );
  return true;
}

bool find(const half_trie_t &half_trie, const char_t *key, uint32_t &value) {
  size_t key_len = strlen(key);
  size_t half_len = key_len / 2;
  uint32_t second_trie_id = 0;
  std::cerr << "prefix key: [" << std::string(key, half_len) << "] second_key: [" << std::string(key + half_len, key_len - half_len) << "]" << std::endl;
  if (!trie_find(half_trie.prefix_trie, key, half_len, second_trie_id)) {
    std::cerr << "no second trie found" << std::endl;
    return false; // no second trie found
  } else if (second_trie_id == 0 and half_len > 0) {
    std::cerr << "no second trie found" << std::endl;
    return false; // no second trie found
  }
  std::cerr << "second trie_id: " << second_trie_id << std::endl;

  trie_t second_trie;
  if (!_get_second_trie_by_id(half_trie, second_trie_id, second_trie)) { 
    return false; // failed to get second trie
  }
        
  // TODO: int second trie by id
  return trie_find(second_trie, &key[half_len], key_len - half_len, value);
}

size_t load_half_trie(void *data, size_t size, half_trie_t &half_trie) {
  // read header
  size_t prefix_size = ((size_t*)data)[0];
  size_t second_size = ((size_t*)data)[1];
  size_t offset_size = ((size_t*)data)[2];

  size_t header_size = sizeof(size_t) * 3;

  // load prefix_trie
  load_trie(((char*)data) + header_size, prefix_size, half_trie.prefix_trie);
  
  half_trie.data_ = data;
  half_trie.size_ = size;  
  half_trie.second_offset = header_size + prefix_size;
  half_trie.offset_list = (size_t*)(((char*)data) +  header_size + prefix_size + second_size);
}
