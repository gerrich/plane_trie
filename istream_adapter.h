#include <iostream>
#include <utility>
#include <stdint.h>

struct istream_adapter_t {
  typedef std::pair<std::string, uint32_t> value_type;

  istream_adapter_t(std::istream &is) : is_(is), lcount_(1) {}

  void push(value_type) {}

  bool empty() const {
    while (next_line_.empty() and is_.good()) {
      std::getline(is_, next_line_);
    }

    if (next_line_.empty()) {
      return true;
    }
    return false;
  }
  const value_type& front() const {
    empty();

    pair_.first = next_line_;
    pair_.second = lcount_;
    return pair_;
  }
  void pop() {
    //read next line
    ++lcount_;
    next_line_.clear();
    empty();
  }

  mutable std::string next_line_;
  uint32_t lcount_;
  mutable std::istream &is_;
  bool eof_;
  mutable value_type pair_;
};
