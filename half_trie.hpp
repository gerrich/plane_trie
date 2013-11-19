#pragma once

#include "plane_trie.hpp"

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
  size_t trie_offset = half_trie.second_offset + _get_offset_by_id(half_trie, second_trie_id);
  size_t trie_size = _get_offset_by_id(half_trie, second_trie_id + 1) - _get_offset_by_id(half_trie, second_trie_id);
  load_trie((void*)((char*)(half_trie.data_) + trie_offset), trie_size, second_trie);
  return true;
}

bool find(const half_trie_t &half_trie, const char_t *key, uint32_t &value) {
  size_t key_len = strlen(key);
  size_t half_len = key_len / 2;
  uint32_t second_trie_id;
  if (!trie_find(half_trie.prefix_trie, key, half_len, second_trie_id)) {
    return false; // no second trie found
  }

  trie_t second_trie;
  if (!_get_second_trie_by_id(half_trie, second_trie_id, second_trie)) { 
    return false; // failed to get second trie
  }
        
  // TODO: int second trie by id
  return trie_find(second_trie, &key[half_len], key_len - half_len, value);
}

size_t load_half_trie(half_trie_t &half_trie, void *data, size_t size) {
  // read header
  size_t prefix_offset = ((size_t*)data)[0];
  size_t second_offset = ((size_t*)data)[1];
  size_t offset_offset = ((size_t*)data)[2];

  size_t prefix_size = second_offset - prefix_offset;
  // load prefix_trie
  load_trie(((char*)data) + prefix_offset, prefix_size, half_trie.prefix_trie);


}
