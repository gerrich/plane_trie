#pragma once

#include "plane_trie.hpp"

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

size_t str_match_len(const char_t *lhs, const char_t* rhs) {
  size_t i = 0;
  while(lhs[i] and lhs[i] == rhs[i]) {
    ++i;
  }
  return i;
}
size_t str_match_len(const std::basic_string<char_t> &lhs, const std::basic_string<char_t> &rhs) {
  size_t i = 0;
  size_t min_len = std::min(lhs.size(), rhs.size());
  while(i < min_len and lhs[i] == rhs[i]) {
    ++i;
  }
  return i;
}



struct build_trie_context_t {
  std::vector<temp_node_t*> node_history;
  std::string last_word;
  size_t offset;
  size_t ignore_count;
  temp_node_t *root_node;

  build_trie_context_t() 
    : offset(0)
    , ignore_count(0)
    , root_node(NULL)
  {
    root_node = new temp_node_t;
    node_history.push_back(root_node);
  }

  ~build_trie_context_t() {
    delete root_node; 
  }
};


template <typename file_t>
void build_trie_push(file_t &file, build_trie_context_t &context, const std::string &word, const value_t &value)
{

  // go up
  size_t match_len = str_match_len(word, context.last_word);

  for (size_t pos = context.last_word.size(); pos > match_len; --pos) {
    size_t write_size = save_to_file(file, context.offset, context.node_history[pos]);
    delete context.node_history[pos]; // delete saved nodes
    context.node_history[pos - 1]->ptr_list.back() = NULL; // set null pointer there
    context.node_history[pos - 1]->offset_list.back() = context.offset; // save offset of saved node
    context.offset += write_size;
  }

  // go down
  // add node(s)
  context.node_history.resize(word.size() + 1);
  for (; match_len < word.size(); ++match_len) {
    temp_node_t *new_node = new temp_node_t;
    context.node_history[match_len]->key_list.push_back(word[match_len]);
    context.node_history[match_len]->offset_list.push_back(0);
    context.node_history[match_len]->ptr_list.push_back(new_node);
    context.node_history[match_len + 1] = new_node;
  }
  // set node_data
  context.node_history[word.size()]->value = value;

  // remember last added word
  context.last_word = word;
}


//returns root offset
template <typename file_t>
size_t build_trie_commit(file_t &file, build_trie_context_t &context) {
  size_t match_len = context.last_word.size();
  while (match_len > 0) {
    size_t write_size = save_to_file(file, context.offset, context.node_history[match_len]);
    delete context.node_history[match_len]; // delete saved nodes
    context.node_history[match_len - 1]->ptr_list.back() = NULL; // set null pointer there
    context.node_history[match_len - 1]->offset_list.back() = context.offset; // save offset of saved node
    context.offset += write_size;
    --match_len;
  }

  size_t root_offset = context.offset;
  size_t write_size = save_to_file(file, context.offset, context.root_node);
  context.offset += write_size;
  return root_offset;
}

// save nodes in post-order from sorted word list 
// returns root node offset
template <typename file_t, typename word_list_t>
size_t build_trie(file_t &file, word_list_t &word_list) {
  build_trie_context_t context;
  //context.root_node = new temp_node_t;
  //context.node_history.push_back(context.root_node);

  while( not word_list.empty()) {

    typename word_list_t::value_type const &pair = word_list.front();
    const std::string &word = pair.first;
    const value_t &value = pair.second;

    // check word order: word > last_word
    if (std::less<std::string>().operator()(word, context.last_word)) {
      std::cerr << "disorder: [" << context.last_word << "][" << word <<"]" << std::endl;
      word_list.pop();
      ++context.ignore_count;
      continue;
    }

    build_trie_push(file, context, word, value);

    word_list.pop();
  }
 
  size_t root_offset = build_trie_commit(file, context);

  BOOST_STATIC_ASSERT(sizeof(root_offset) == 8);
  _write(file, &root_offset, sizeof(root_offset));
  
  
  return root_offset;
//  offset += write_size;
}


