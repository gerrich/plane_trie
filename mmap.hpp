#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

class mmap_t {
public:
  mmap_t() {}

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
    if ((data_ = mmap (0, size_, PROT_READ, MAP_POPULATE|MAP_PRIVATE, fd_, 0)) == (caddr_t) -1)
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
