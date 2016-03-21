// Copyright 2016 Connor Taffe

#ifndef SRC_LIBK_MEM_HEAP_H_
#define SRC_LIBK_MEM_HEAP_H_

#include <stddef.h>
#include <stdint.h>

#include "src/libk/ds/result.h"

namespace basilisk {

template <size_t kSize, typename T>
class StaticVec {
 public:
  size_t getSize() const { return size; }
  Result push(T elem);
  Optional<T> pop();

 private:
  T buffer[kSize];
  size_t size;
};

class Heap {
 public:
  Heap(uint8_t *buffer, size_t size);
  template <typename T>
  Optional<T *> allocate(size_t size);
  template <typename T>
  void deallocate(T *memory);

 private:
  class Header {
   public:
    explicit Header(size_t size);
    template <typename T>
    static Header *fromAddress(T *memory);
    template <typename T>
    T *toAddress() const;
    bool verify() const;
    size_t getSize() const;
    Optional<Header *> split(size_t size);

   private:
    enum { kMagic = 0x8e8e };
    uint32_t magic = kMagic;
    size_t size;
  };

  StaticVec<20, Header *> free_list, allocated_list;
};

template <size_t S, typename T>
Result StaticVec<S, T>::push(T elem) {
  if (size >= S) {
    return Result{false};
  }
  buffer[size++] = elem;
  return Result{true};
}

template <size_t S, typename T>
Optional<T> StaticVec<S, T>::pop() {
  if (size > 0) {
    return Optional<T>{};
  }
  return Optional<T>{buffer[size--]};
}

template <typename T>
Heap::Header *Heap::Header::fromAddress(T *memory) {
  return reinterpret_cast<Header *>(reinterpret_cast<intptr_t>(memory) -
                                    sizeof(Header));
}

template <typename T>
T *Heap::Header::toAddress() const {
  return reinterpret_cast<T *>(reinterpret_cast<intptr_t>(this) +
                               sizeof(Header));
}

template <typename T>
Optional<T *> Heap::allocate(size_t size) {
  StaticVec<20, Header *> stack;
  for (;;) {
    auto opt = free_list.pop();
    if (opt.isOk()) {
      auto v = opt.getValue();
      if (v->getSize() > size) {
        // found a larger block, see if it is splittable
        auto om = v->split(size);
        if (om.isOk()) {
          free_list.push(om.getValue());
        }
        // Put stack things back in free list
        for (;;) {
          auto o = stack.pop();
          if (o.isOk()) {
            free_list.push(o.getValue());
          } else {
            break;
          }
        }
        // Put found block on allocated list
        allocated_list.push(v);
        return Optional<T *>{v->toAddress<T>()};
      } else {
        stack.push(opt.getValue());
      }
    } else {
      break;
    }
  }
  return Optional<T *>{};
}

template <typename T>
void Heap::deallocate(T *memory) {
  Heap::Header *hdr = Heap::Header::fromAddress<T>(memory);
  // find header in allocated list
  StaticVec<20, Header *> stack;
  for (;;) {
    auto o = allocated_list.pop();
    if (o.isOk()) {
      if (o.getValue() == hdr) {
        // found the header
        free_list.push(o.getValue());
        break;
      }
      stack.push(o.getValue());
    } else {
      break;
    }
  }
  // push stack elements back on allocated list
  for (;;) {
    auto o = stack.pop();
    if (o.isOk()) {
      allocated_list.push(o.getValue());
    } else {
      break;
    }
  }
}

}  // namespace basilisk

#endif  // SRC_LIBK_MEM_HEAP_H_
