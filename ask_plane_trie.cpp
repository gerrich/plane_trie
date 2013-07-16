#include <iostream>
#include "plane_trie.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

class mmap_t {
public:
  mmap_t(const char* fname) {
    struct stat statbuf;

    /* open the input file */
    if ((fd_ = open (fname, O_RDONLY)) < 0)
      throw 1; // err_sys ("can't open %s for reading", argv[1]);

    /* find size of input file */
    if (fstat (fd_, &statbuf) < 0)
      throw 2; //err_sys ("fstat error");

    size_ = statbuf.st_size;

    /* mmap the input file */
    if ((data_ = mmap (0, size_, PROT_READ, MAP_PRIVATE, fd_, 0)) == (caddr_t) -1)
      throw 3; //err_sys ("mmap error for input");
  }

  ~mmap_t() {
    if (munmap(data_, size_) == -1) {
      // err
    }
    close(fd_);
  }

  void* data() {return data_;}
  const void* data() const {return data_;}
  size_t size() {return size_;}

private:
  int fd_;
  void *data_;
  size_t size_;
};

int main(int argc, char**argv) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <trie.bin>" << std::endl;
    return 1;
  }

  mmap_t my_map(argv[1]);
 
  trie_t trie;
  load_trie(my_map.data(), my_map.size(), trie);

  while(std::cin.good()) {
    std::string line;
    getline(std::cin, line);
    if (line.empty()) continue;

    uint32_t value = get_node(trie, line.c_str());
    std::cout << line << "\t" << value << std::endl;
  }

  return 0;
}
