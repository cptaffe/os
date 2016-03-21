// Copyright 2016 Connor Taffe

#ifndef SRC_VEC_H_
#define SRC_VEC_H_

#include <stddef.h>
#include <stdint.h>

namespace basilisk {

template <typename T>
class Vec {
 public:
  ~Vec();
  size_t getSize() const { return size; }
  void push(T elem);
  T pop();
  void erase(size_t i);
  bool empty();
  T &operator[](size_t i);

  class Iter {
   public:
    explicit Iter(Vec *v, size_t i) : vec{v}, index{i} {}
    void operator++() {
      if (index < vec->getSize()) {
        index++;
      }
    }
    bool operator!=(const Iter &other) { return index != other.index; }
    T &operator*() { return (*vec)[index]; }

   private:
    Vec *vec;
    size_t index = 0;
  };

  Iter begin() { return Iter{this, 0}; }
  Iter end() { return Iter{this, size}; }

 private:
  T *buffer = nullptr;
  size_t size = 0, allocated = 0;

  virtual void realloc(size_t new_size);
};

template <typename T>
Vec<T>::~Vec() {
  if (buffer != nullptr) {
    delete buffer;
  }
}

template <typename T>
void Vec<T>::push(T elem) {
  realloc(size + 1);
  buffer[++size] = elem;
}

template <typename T>
T Vec<T>::pop() {
  T elem = buffer[--size];
  realloc(size);
  return elem;
}

template <typename T>
void Vec<T>::erase(size_t i) {
  if (i >= size) return;
  for (size_t j = i + 1; j < size; j++) {
    buffer[j - 1] = buffer[j];
  }
  realloc(--size);
}

template <typename T>
void Vec<T>::realloc(size_t ns) {
  if (ns < allocated / 2) {
    ns = allocated / 2;
  } else if (ns > allocated) {
    ns = allocated * 2 + 1;
  } else {
    return;  // noop
  }
  T *nbuf = reinterpret_cast<T *>(new uint8_t[ns * sizeof(T)]);
  for (size_t i = 0; i < size; i++) {
    nbuf[i] = buffer[i];
  }
  allocated = ns;
  delete buffer;
  buffer = nbuf;
}

template <typename T>
bool Vec<T>::empty() {
  return size == 0;
}

template <typename T>
T &Vec<T>::operator[](size_t i) {
  if (i >= size) {
    // TODO(cptaffe): throw error
  }
  return buffer[i];
}

}  // namespace basilisk

#endif  // SRC_VEC_H_
