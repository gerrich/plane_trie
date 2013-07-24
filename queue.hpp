#pragma once

// 4 times faster then std::queue<data_t, std::deque<data_t> >

enum _reuse_policy_t {
  NO_REUSE = 0,
  REUSE = 1
};

template <typename T, int block_size>
struct _queue_node_t {
  _queue_node_t *next_;
  char data_[sizeof(T) * block_size];
};

template <typename T, int block_size = 1000, _reuse_policy_t reuse_policy = REUSE>
class queue_t {
public:
  typedef _queue_node_t<T, block_size> node_t;
  
  queue_t() {
    head_ = new node_t;
    head_->next_ = NULL;    
    tail_ = head_;
    head_offset_ = 0;
    tail_offset_ = 0;
  }
  ~queue_t() {
    while (head_) {
      node_t *tmp = head_->next_;
      delete head_;
      head_ = tmp;
    }
  }

  size_t size() const {
    const node_t *node = head_;
    size_t count = 0;
    for (; node->next_; node = node->next_) {
      count += 1;
    }
    count *= block_size;
    count += tail_offset_ - head_offset_;
    return count;
  }
  bool empty() const {
    return (head_ == tail_ and head_offset_ == tail_offset_);
  }
  void push(const T& t) {
    if (tail_offset_ == block_size) {
      if (reuse_policy == REUSE and tail_->next_) {
        tail_ = tail_->next_;
      } else {
        tail_->next_ = new node_t;
        tail_ = tail_->next_;
        tail_->next_ = NULL;
      }
      tail_offset_ = 1;
      ((T*)(tail_->data_))[0] = t;
    } else {
      ((T*)(tail_->data_))[tail_offset_] = t;
      ++tail_offset_;
    }
  }
  void pop() {
    if (head_offset_ + 1 == block_size) {
      if (head_ == tail_) {
        tail_offset_ = 0;
        head_offset_ = 0;
      } else if (head_->next_) {
        node_t *tmp = head_;
        head_ = head_->next_;
        head_offset_ = 0;

        if (reuse_policy == REUSE) {
          tmp->next_ = tail_->next_;
          tail_->next_ = tmp;
        } else {
          delete tmp;
        }
      }
    } else {
      ++head_offset_;
    }
  }
  const T& front() const {
    return ((const T*)(head_->data_))[head_offset_];
  }
  T& front() {
    return ((T*)(head_->data_))[head_offset_];
  }
  const T& back() const {
    return ((const T*)(tail_->data_))[tail_offset_ - 1];
  }
  T& back() {
    return ((T*)(tail_->data_))[tail_offset_ - 1];
  }

private:
  node_t *head_;
  node_t *tail_;
  size_t head_offset_;
  size_t tail_offset_;
};


