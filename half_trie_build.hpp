#pragma once
#include "half_trie.hpp"
#include "plane_trie_build.hpp"

bool str_eq(const char_t *s1, size_t l1, const char_t* s2, size_t l2) {
  if (l1 != l2) return false;
  for (size_t i = 0; i < l1 /*&& i < l2*/; ++i) {
    if (s1[i] != s2[i]) {
      return false; 
    }
  }
  return true;
}

bool split(const std::string &s, char sep, std::string &s1, std::string &s2) {
  size_t pos = s.find(sep); 
  if (pos == -1) return false;
  s1.assign(s, 0, pos);
  size_t offset = pos + 1;
  if (offset >= s.size()) {
    s2.clear();
    return true;
  }
  pos = s.find("sep", offset);
  if (pos == -1) pos = s.size();
  s2.assign(s, offset, pos - offset);
  return true;
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

//  const char_t *old_prefix_word;
//  size_t old_prefix_len = 0;
  std::string old_prefix;
  uint32_t prefix_id = 0;

  build_trie_context_t prefix_context;
  build_trie_context_t second_context;
  
//  build_trie_init(second_file, prefix_context);
//  build_trie_init(second_file, second_context);

  while( not word_list.empty()) {
    typename word_list_t::value_type const &pair = word_list.front();
    const value_t &value = pair.second;
    std::string prefix;
    std::string second;

    if (!split(pair.first, '\t', prefix, second)) {
      word_list.pop();
      continue;
    }

    std::cerr << "pfx [" << prefix << "] sfx[" << second << "]" << std::endl; 
    if (prefix == old_prefix) {
      // add new node to second trie
      build_trie_push(second_file, second_context, second, value);
    } else {
      // commit second_trie
      size_t second_root_offset = build_trie_commit(second_file, second_context);
      // commit_offset
      _write(offset_file, &second_root_offset, sizeof(second_root_offset));

      ++prefix_id;
      // push to prefix_trie
      build_trie_push(prefix_file, prefix_context, prefix, prefix_id);
      // init second trie
      {
        size_t offset = second_context.offset;
        second_context = build_trie_context_t();
        second_context.offset = offset;
      }
      //build_trie_init(second_file, second_context);
      // push to second trie
      build_trie_push(second_file, second_context, second, value);

    }

    old_prefix = prefix;
      
    word_list.pop();
  }

  // commit second_trie
  size_t second_root_offset = build_trie_commit(second_file, second_context);
  // commit_offset
  _write(offset_file, &second_root_offset, sizeof(second_root_offset));
  
  // commit prefix_trie
  size_t prefix_root_offset = build_trie_commit(prefix_file, prefix_context);
  // commit_prefix_root_offset
  _write(prefix_file, &prefix_root_offset, sizeof(prefix_root_offset));

  return prefix_root_offset;
}

// TODO: error code
template <typename file_t>
void copy_to_file(file_t file, FILE*f, size_t size) {
  const size_t BUF_SIZE = 1024*16;
  char *buffer = new char[BUF_SIZE];
  size_t offset = BUF_SIZE;
  for (; offset < size; ++offset) {
    fread(buffer, 1, BUF_SIZE, f);
    _write(file, buffer, BUF_SIZE);
  }
  fread(buffer, 1, size + BUF_SIZE - offset, f);
  _write(file, buffer, size + BUF_SIZE - offset);

  delete [] buffer;
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

  // write header
  _write(file, &prefix_size, sizeof(size_t));
  _write(file, &second_size, sizeof(size_t));
  _write(file, &offset_size, sizeof(size_t));

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


