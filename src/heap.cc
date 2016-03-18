// Copyright 2016 Connor Taffe

#include "src/heap.h"

namespace basilisk {

Heap::Heap(uint8_t *buffer, size_t size) {
  auto h = reinterpret_cast<Header *>(buffer);
  *h = Header{size};
  free_list.push(h);
}

Heap::Header::Header(size_t sz) : size{sz} {}

bool Heap::Header::verify() const { return magic == kMagic; }

size_t Heap::Header::getSize() const { return size; }

// \param sz: the size of the memory buffer to allocate (without headers)
Optional<Heap::Header *> Heap::Header::split(size_t sz) {
  if (sz + (2 * sizeof(Header)) > size) {
    // can split
    // point new header to new header location
    Header *new_header = reinterpret_cast<Header *>(
        reinterpret_cast<intptr_t>(this) + sz + sizeof(Header));
    // | Header | size bytes | *new_header
    *new_header = Header{size - (sz + static_cast<size_t>(sizeof(Header)))};
    size = sz + sizeof(Header);
    return Optional<Header *>{new_header};
  }
  return Optional<Header *>{};
}

}  // namespace basilisk
