#include <utility>
#include <stdint.h>
#include <cstdio>
#include <string>
#include <cstring>

struct file_adapter_t {
  typedef std::pair<std::string, uint32_t> value_type;

  file_adapter_t(FILE* file) : len_(0), file_(file), lcount_(1) {
    buffer_size_ = 10500; 
    buffer_ = new char[buffer_size_];
  }

  ~file_adapter_t() {
    delete [] buffer_;
  }

//  void push(value_type) {}

  bool empty() const {
    while (len_ == 0 and !feof(file_)) {
      if (!fgets(buffer_, buffer_size_, file_)) {
        eof_ = true;
        buffer_[0] = '\0';
        len_ = 0;
        return true;
      } else {
        len_ = strlen(buffer_);
        if (buffer_[len_-1] == '\n') {
          buffer_[len_ - 1] = '\0';
          len_ -= 1;
        }
      }
    }

    if (!len_) { // remove me
      return true;
    }
    return false;
  }
  const value_type& front() const {
    empty();

    pair_.first = buffer_;
    pair_.second = lcount_;
    return pair_;
  }
  void pop() {
    //read next line
    ++lcount_;
    len_ = 0;
    empty();
  }

  size_t buffer_size_;
  mutable char *buffer_;
  mutable size_t len_;

  uint32_t lcount_;
  mutable FILE *file_;
  mutable bool eof_;
  mutable value_type pair_;
};
