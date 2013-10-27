#pragma once

#include <plane_trie.hpp>

struct half_trie_t {
  trie_t prefix_trie;

  // TODO: second trie storage
};


bool find(const half_trie_t &half_trie, const char_t *key, uint32_t &value) {
  size_t key_len = strlen(key);
  size_t half_len = key_len / 2;
  uint32_t second_trie_id;
  if (!trie_find(half_trie.prefix_trie, key, half_len, second_trie_id)) {
    return false; // no second trie found
  }

  trie_t second_trie;
  // TODO: int second trie by id
  return trie_find(half_trie.prefix_trie, &key[half_len], len - half_len, value);
}


//returns root node offset
template <typename file_t, typename word_list_t>
size_t build_half_trie(
    file_t &prefix_file,
    file_t &second_file,
    file_t &offset_file,
    word_list_t &word_list
) {
  // check order!  

  char *old_prefix;
  char *old_prefix_len;
  uint32_t prefix_id = 0;

  while( not word_list.empty()) {
    char_t *prefix;
    size_t prefix_len;
     
    char_t * second;
    size_t second_len;

    value_t &value;

    if (strcmp(prefix, prefix_len, old_prefix, old_prefix_len)) {
      // add new node to second trie
      build_trie_push(second_file, second_context, second_word, value);
    } else {
      // commit second_trie
      size_t second_root_offset = build_trie_commit(second_file, second_context);
      // commit_offset
      save_to_file(offset_file, second_root_offset, sizeof(second_root_offset));

      ++prefix_id;
      // push to prefix_trie
      build_trie_push(prefix_file, prefix_context, prefix_word, prefix_id);
      // init second trie
      build_trie_init(second_file, second_context);
      // push to second trie
      build_trie_push(second_file, second_context, second_word, value);
    }
    // commit second_trie
    size_t second_root_offset = build_trie_commit(second_file, second_context);
    // commit_offset
    save_to_file(offset_file, second_root_offset, sizeof(second_root_offset));
    
    // commit prefix_trie
    size_t prefix_root_offset = build_trie_commit(prefix_file, prefix_context);
    // commit_prefix_root_offset
//    save_to_file(offset_file, second_root_offset, sizeof(second_root_offset));
  }
  return prefix_root_offset;
}

// TODO: error code
template <typename file_t>
void copy_to_file(file_t file, FILE*f, size_t size) {
  const size_t BUF_SIZE = 1024*16;
  char *buffer = malloc(BUF_SIZE);
  size_t offset = BUF_SIZE;
  for (; oofset < size; ++offset) {
    fread(buffer, 1, BUF_SIZE, f);
    _write(file, buffer, BUF_SIZE);
  }
  fread(buffer, 1, size + BUF_SIZE - offset, f);
  _write(file, buffer, size + BUF_SIZE - offset);

  free(buffer);
}

template <typename file_t, typename word_list_t>
size_t build_half_trie(file_t file, word_list_t &word_list) {
  // make 3 tmp files
  FILE *prefix_file = tmpfile();
  FILE *second_file = tmpfile();
  FILE *offset_file = tmpfile();

  size_t root_offset = build_half_trie(prefix_file, second_file, offset_file, word_list);
  // cat 3 file to file
  long int prefix_size = ftell(prefix_file);
  long int second_size = ftell(second_file);
  long int offset_size = ftell(offset_file);
  fseek(prefix_file, 0, SEEK_SET);
  fseek(second_file, 0, SEEK_SET);
  fseek(offset_file, 0, SEEK_SET);

  copy_to_file(file, prefix_file, prefix_size);
  copy_to_file(file, second_file, second_size);
  copy_to_file(file, offset_file, offset_size);

  // rm 3 files
  fclose(prefix_file);
  fclose(second_file);
  fclose(offset_file);
  
  // commit_root_offset
  return root_offset;
}

size_t load_half_trie(void *data, size_t size) {
  // read header
  // load prefix_trie
  // 
}
