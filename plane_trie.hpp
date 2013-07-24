#pragma once 

#include <stdint.h>
#include <cstdio>

#include <map>
#include <queue>
#include <vector>
#include <list>
#include <string>
#include <ostream>
#include <algorithm>
#include <iostream>
#include <boost/static_assert.hpp>

#include "queue.hpp"

// fast load trie
// build from sorted string list
// fast read only access
typedef char char_t;
typedef uint32_t value_t; // word_id

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

struct trie_t {
  plane_node_t *root_node;
  size_t segment_size;  
  void* addr;
};

inline size_t round_up_8(size_t size) {
  return ((size % 8) ? (size - size % 8 + 8) : (size));
//  return ((size & (not size_t(0x07))) | ((size & size_t(7)) ? 8 : 0));
}
inline size_t round_up_4(size_t size) {
  return ((size & (not size_t(0x03))) | ((size & size_t(3)) ? 4 : 0));
}

BOOST_STATIC_ASSERT(sizeof(plane_node_t) == 8);

size_t calc_node_size(const temp_node_t *node) {
  return sizeof(plane_node_t) + node->offset_list.size() * sizeof(void*) + round_up_8(node->key_list.size());
}

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

// save nodes in post-order from sorted word list 
// returns root node offset
template <typename file_t, typename word_list_t>
size_t build_trie(file_t &file, word_list_t &word_list) {
  std::string last_word;
  size_t ignore_count = 0;
  size_t offset = 0;

  temp_node_t *root_node = new temp_node_t;

  std::vector<temp_node_t*> node_history;
  node_history.push_back(root_node);

  while( not word_list.empty()) {

    typename word_list_t::value_type const &pair = word_list.front();
    const std::string &word = pair.first;
    const value_t &value = pair.second;

    // check word order: word > last_word
    if (std::less<std::string>().operator()(word, last_word)) {
      std::cerr << "disorder: [" << last_word << "][" << word <<"]" << std::endl;
      word_list.pop();
      ++ignore_count;
      continue;
    }
    // go up
    size_t match_len = str_match_len(word, last_word);

    for (size_t pos = last_word.size(); pos > match_len; --pos) {
      size_t write_size = save_to_file(file, offset, node_history[pos]);
      delete node_history[pos]; // delete saved nodes
      node_history[pos - 1]->ptr_list.back() = NULL; // set null pointer there
      node_history[pos - 1]->offset_list.back() = offset; // save offset of saved node
      offset += write_size;
    }

    // go down
    // add node(s)
    node_history.resize(word.size() + 1);
    for (; match_len < word.size(); ++match_len) {
      temp_node_t *new_node = new temp_node_t;
      node_history[match_len]->key_list.push_back(word[match_len]);
      node_history[match_len]->offset_list.push_back(0);
      node_history[match_len]->ptr_list.push_back(new_node);
      node_history[match_len + 1] = new_node;
    }
    // set node_data
    node_history[word.size()]->value = value;

    // remember last added word
    last_word = word;
    word_list.pop();
  }
  
  size_t match_len = last_word.size();
  while (match_len > 0) {
    size_t write_size = save_to_file(file, offset, node_history[match_len]);
    delete node_history[match_len]; // delete saved nodes
    node_history[match_len - 1]->ptr_list.back() = NULL; // set null pointer there
    node_history[match_len - 1]->offset_list.back() = offset; // save offset of saved node
    offset += write_size;
    --match_len;
  }

  size_t root_offset = offset;
  size_t write_size = save_to_file(file, offset, root_node);

  BOOST_STATIC_ASSERT(sizeof(root_offset) == 8);
  _write(file, &root_offset, sizeof(root_offset));
  
  
  return root_offset;
//  offset += write_size;
}


// load trie structure from mapped region
void load_trie(void* addr, size_t size, trie_t& trie) {
  size_t root_offset = *(size_t*)((char*)(addr) + size - sizeof(void*));
  trie.addr = addr;
  trie.segment_size = size;
  trie.root_node = reinterpret_cast<plane_node_t*>((char*)addr + root_offset);
}

inline const char_t* _get_key_begin(const plane_node_t *node) {
  return (char_t*)((char*)(node) + sizeof(plane_node_t));
}
inline const size_t* _get_offset_begin(const plane_node_t *node) {
  return (size_t*)((char*)(node) + sizeof(plane_node_t) + round_up_8(node->child_count));
}

inline const plane_node_t* _get_node(const trie_t& trie, size_t offset) {
  return (const plane_node_t*)((char*)trie.addr + offset);
} 

struct less_t {
  inline bool operator ()(const char_t &lhs, const char_t& rhs) const {
    return (uint8_t)lhs < (uint8_t)rhs;
  }
};

bool _find_child_node(const trie_t &trie, const plane_node_t *curr_node, const char_t &key, const plane_node_t **next_node) {
  char_t *key_begin = (char_t*)((char*)(curr_node) + sizeof(plane_node_t));
  char_t *key_end = key_begin + curr_node->child_count;
  static less_t _less;
  char_t *key_ptr = std::lower_bound(key_begin, key_end, key, _less);
  if (key_end == key_ptr or *key_ptr != key) {
    return false;
  }
  size_t *offset_begin = (size_t*)((char*)(curr_node) + sizeof(plane_node_t) + round_up_8(curr_node->child_count));
  size_t new_offset = *(offset_begin + (size_t)(key_ptr - key_begin));
  *next_node = _get_node(trie, new_offset); //(const plane_node_t*)((char*)trie.addr + new_offset);
  return true;
}

value_t get_node(const trie_t &trie, const char_t* word) {
  BOOST_STATIC_ASSERT(sizeof(size_t) == sizeof(void*));
  const plane_node_t *curr_node = trie.root_node;
  for (size_t i = 0; word[i] != 0; ++i) {
    if (!_find_child_node(trie, curr_node, word[i], &curr_node)) return 0;
  }
  return curr_node->value;
}

struct task_t {
  size_t word_pos;
  size_t dict_pos;
  size_t ttl;
  const plane_node_t *node;
};

template <typename output_t>
void fuzzy_search(const trie_t &trie, const char_t* word, size_t max_dist, output_t &output) {
  //task: word_pos, dict_pos, ttl
  queue_t<task_t, (sizeof(task_t) * 1024 - sizeof(void*)) / sizeof(task_t)> q;
  //std::queue<task_t> q;
  q.push((task_t){0, 0, max_dist, trie.root_node});

  while(!q.empty()) {
    const task_t &task = q.front();

    if (task.ttl) {
      // delete
      // foreach curr_node keys
      const size_t *offset_begin = _get_offset_begin(task.node);
      for (size_t i = 0; i < task.node->child_count; ++i) {
        q.push(task);
        q.back().ttl -= 1;
        q.back().dict_pos += 1;
        q.back().node = _get_node(trie, offset_begin[i]);
      }

      // insert
      if (task.ttl and word[task.word_pos]) {
        q.push(task);
        q.back().ttl -= 1;
        q.back().word_pos += 1;
      }

      // replace
      // foreach curr_node keys
      if (word[task.word_pos]) {
        for (size_t i = 0; i < task.node->child_count; ++i) {
          const char_t &key = ((char_t*)((char*)task.node + sizeof(plane_node_t)))[i];
          if (key == word[task.word_pos]) {
            continue; // ignore replace if it's the same as paste
          } 
          q.push(task);
          q.back().ttl -= 1;
          q.back().dict_pos += 1;
          q.back().word_pos += 1;
          q.back().node = _get_node(trie, offset_begin[i]);
        }
      }
      // transpose
      // check two positions
    }
    
    bool no_pop = false;
    if (word[task.word_pos]) {
      // paste
      const plane_node_t *next_node;
      if (_find_child_node(trie, task.node, word[task.word_pos], &next_node)) {
        no_pop = true;
        task_t &new_task = q.front();
//        q.push(task);
//      task_t &new_task = q.back();  
        new_task.dict_pos += 1;
        new_task.word_pos += 1;
        new_task.node = next_node;
      }
    } else {
      // report found word
      if (task.node->value) {
        output.insert(task.node->value);
//        std::cout << "FOUND: " << task.node->value << std::endl;
      }
    }
    if (not no_pop)
      q.pop();
  }
}



struct _fuzzy_search_task_t {
  const plane_node_t *dict_node;
  const plane_node_t *word_node;
  size_t ttl;
};

struct _fuzzy_search_greater_task_t {
  bool operator() (const _fuzzy_search_task_t& lhs, const _fuzzy_search_task_t& rhs) const {
    // descendents have lower addreses tan their parents
    // siblings go in key order
    return 
      lhs.word_node > rhs.word_node or
      lhs.word_node == rhs.word_node and (
        lhs.dict_node > rhs.dict_node or
        lhs.dict_node == rhs.dict_node and
        lhs.ttl < rhs.ttl
      );
  }
};

inline bool _empty_node(const plane_node_t* node) {
  return (node->value == 0 and node->child_count == 0);
}

// get fuzy intersection of two tries
// lower_limit - lowest word in task trie
// upper_limit - a word after largest word in task trie
//
// output format: pairs: word_id -> dict_id
template <typename output_t>
void fuzzy_search(
    const trie_t &dict_trie,
    const trie_t &word_trie,
    const char_t *lower_limit,
    const char_t *upper_limit,
    size_t max_dist,
    output_t &output) {

  less_t _less;

  typedef _fuzzy_search_task_t task_t;
  typedef _fuzzy_search_greater_task_t greater_task_t;

  struct equal_task_t {
  bool operator()(const task_t &lhs, const task_t &rhs) {
    return 
      lhs.word_node == rhs.word_node and 
      lhs.dict_node == rhs.dict_node;// and
//      lhs.ttl <= rhs.ttl;
  }
  };

  //queue_t<task_t, (sizeof(task_t) * 1024 - sizeof(void*)) / sizeof(task_t)> q;
  std::priority_queue<task_t, typename std::vector<task_t>, greater_task_t> q;
  q.push((task_t){dict_trie.root_node, word_trie.root_node, max_dist});

//  size_t step_no = 0;
//  size_t skip_no = 0;
  while (! q.empty() ) {
    //++step_no;
    const task_t task = q.top();
    q.pop();

//    while (! q.empty() and equal_task_t()(q.top(), task)) {
//      ++skip_no;
//      q.pop();
//    }

//    if (step_no % 10000 == 0) {
//      std::cout << "step_no:" << step_no << "\tq_size:" << q.size() << "\tskip_no:" << skip_no <<"\t"<<task.word_node << "\t"<<task.dict_node  << "\t" << task.ttl<< std::endl;
//    }

    const size_t *dict_offset_begin = _get_offset_begin(task.dict_node);
    const size_t *word_offset_begin = _get_offset_begin(task.word_node);
    
    if (task.ttl) {
      //delete
      for (size_t i = 0; i < task.dict_node->child_count; ++i) {
        const plane_node_t *dict_child = _get_node(dict_trie, dict_offset_begin[i]);
        if (_empty_node(dict_child)) continue;
        task_t new_task = task;
        new_task.ttl -= 1;
        new_task.dict_node = dict_child;
        q.push(new_task);
      }

      // insert
      for (size_t i = 0; i < task.word_node->child_count; ++i) {
        const plane_node_t *word_child = _get_node(word_trie, word_offset_begin[i]);
        if (_empty_node(word_child)) continue;
        task_t new_task = task;
        new_task.ttl -= 1;
        new_task.word_node = word_child;
        q.push(new_task);
      }

      // replace
      // foreach curr_node keys
      for (size_t dict_child_id = 0; dict_child_id < task.dict_node->child_count; ++dict_child_id) {
        const plane_node_t *dict_child = _get_node(dict_trie, dict_offset_begin[dict_child_id]);
        if (_empty_node(dict_child)) continue;

        for (size_t word_child_id = 0; word_child_id < task.word_node->child_count; ++word_child_id) {
          const plane_node_t *word_child = _get_node(word_trie, word_offset_begin[word_child_id]);
          if (_empty_node(word_child)) continue;

          const char_t &dict_key = ((char_t*)((char*)task.dict_node + sizeof(plane_node_t)))[dict_child_id];
          const char_t &word_key = ((char_t*)((char*)task.word_node + sizeof(plane_node_t)))[word_child_id];
          
          if (dict_key == word_key) {
            continue; // ignore replace if it's the same as paste
          }

          task_t new_task = task;
          new_task.ttl -= 1;
          new_task.dict_node = dict_child;
          new_task.word_node = word_child;
          q.push(new_task);
        }
      }
    }
    
    // paste
    for (size_t i = 0, j = 0; i < task.dict_node->child_count && j < task.word_node->child_count; ) {
      const char_t &dict_key = ((char_t*)((char*)task.dict_node + sizeof(plane_node_t)))[i];
      const char_t &word_key = ((char_t*)((char*)task.word_node + sizeof(plane_node_t)))[j];

      if (_less(dict_key, word_key)) {
        ++i;
        continue;
      } else if (_less(word_key, dict_key)) {
        ++j;
        continue;
      } else {
        const plane_node_t *dict_child = _get_node(dict_trie, dict_offset_begin[i]);
        if (_empty_node(dict_child)) {
          ++i;
          ++j;
          continue;
        }
        
        const plane_node_t *word_child = _get_node(word_trie, word_offset_begin[j]);
        if (_empty_node(word_child)) {
          ++i;
          ++j;
          continue;
        }
        //std::cout << "EQ " << i << " " << j << " : " << word_key << " " << dict_key << std::endl; 
        task_t new_task = task;
        new_task.dict_node = dict_child;
        new_task.word_node = word_child;
        q.push(new_task);
       
        ++i;
        ++j;
      }

    }

    if (task.dict_node->value && task.word_node->value) {
      output.insert(std::make_pair(task.word_node->value, task.dict_node->value));
    }
  }
}

//void save_trie(std::ostream &os, const trie_t& trie);

