// Copyright 2016 Connor Taffe

#ifndef SRC_LIBK_STD_H_
#define SRC_LIBK_STD_H_

#include <stddef.h>
#include <stdint.h>

#include "src/libk/kernel.h"

// placement new & delete
inline void *operator new(size_t, void *p) { return p; }
inline void *operator new[](size_t, void *p) { return p; }
inline void operator delete(void *, void *) {}
inline void operator delete[](void *, void *) {}

namespace std {

template <class T>
class initializer_list {
 public:
  constexpr initializer_list(const T *b, size_t s) : data_{b}, size_{s} {}
  constexpr size_t size() const { return size_; }
  constexpr T operator[](size_t i) const { return data_[i]; }

  class Iter {
   public:
    constexpr Iter(initializer_list *a, size_t i = 0) : list_{a}, index{i} {}
    void operator++() {
      if (index < list_->size()) {
        index++;
      }
    }
    T *operator*() const { return &(*list_)[index]; }
    bool operator!=(const Iter &other) const {
      return other.list_ != list_ || other.index != index;
    }

   private:
    initializer_list *list_;
    size_t index;
  };

  constexpr Iter begin() { return Iter{this}; }
  constexpr Iter end() { return Iter{this, size()}; }

 private:
  const T *data_;
  size_t size_;
};

template <class T, size_t S>
class array {
 public:
  array() : data_{} {}
  explicit array(initializer_list<T> list) {
    for (size_t i = 0; i < list.size(); i++) {
      data_[i] = list[i];
    }
  }
  constexpr size_t size() const { return S; }
  constexpr bool empty() const { return size() == 0; }
  constexpr T &operator[](size_t i) { return data_[i]; }
  constexpr T *data() { return data_; }
  void fill(const T &v) {
    for (auto &e : *this) {
      e = v;
    }
  }
  void swap(array<T, S> &other) {
    for (size_t i = 0; i < size(); i++) {
      T t = (*this)[i];
      (*this)[i] = other[i];
      other[i] = t;
    }
  }

  class Iter {
   public:
    constexpr Iter(array *a, size_t i = 0) : array_{a}, index{i} {}
    void operator++() {
      if (index < array_->size()) {
        index++;
      }
    }
    T *operator*() const { return &(*array_)[index]; }
    bool operator!=(const Iter &other) const {
      return other.array_ != array_ || other.index != index;
    }

   private:
    array<T, S> *array_;
    size_t index;
  };

  constexpr Iter begin() { return Iter{this}; }
  constexpr Iter end() { return Iter{this, size()}; }

 private:
  T data_[S];
};

template <class... T>
class tuple {
 public:
  explicit tuple(T... types) : list_{types...} {}
  constexpr size_t size() const { return sizeof...(T); }

 private:
  template <class Head, class... Tail>
  class list {
   public:
    list(Head h, Tail... t) : head_{h}, tail_{t...} {}
    constexpr size_t size() const { return sizeof...(Tail) + 1; }
    template <size_t I>
    decltype(auto) get() {
      if (I == 0) {
        return head_;
      } else {
        return tail_.template get<I - 1>();
      }
    }

   private:
    Head head_;
    list<Tail...> tail_;
  };
  template <class Head>
  class list<Head> {
   public:
    explicit list(Head h) : head_{h} {}
    template <size_t I>
    decltype(auto) get() {
      if (I == 0) {
        return head_;
      }
    }

   private:
    Head head_;
  };

  list<T...> list_;

 public:
  template <size_t I>
  decltype(auto) get() {
    return list_.template get<I>();
  }
};

template <class A, class B>
void copy(A dest, B src) {
  auto s = src.begin();
  for (auto &d : dest) {
    d = *s;
    if (s != src.end()) {
      s++;
    } else {
      break;
    }
  }
}

template <class T>
class vector {
 public:
  vector() : capacity_{0} {}
  explicit vector(initializer_list<T> list)
      : capacity_{list.size()},
        size_{list.size()},
        data_{reinterpret_cast<T *>(new uint8_t[list.size() * sizeof(T)])} {
    for (size_t i = 0; i < list.size(); i++) {
      data_[i] = list[i];
    }
  }
  size_t size() const { return size_; }
  bool empty() const { return size() == 0; }
  T &operator[](size_t i) const {
    if (i >= size()) {
      basilisk::term.fatal("index out of bounds of vector");
    }
    return data_[i];
  }
  T *data() const { return data_; }
  void push(T e) {
    if (size_ == capacity_) {
      auto od = data_;
      data_ = reinterpret_cast<T *>(
          new uint8_t[(capacity_ = capacity_ * 2 + 1) * sizeof(T)]);
      for (size_t i = 0; i < size(); i++) {
        data_[i] = od[i];
      }
      delete[] od;
    }
    data_[size_++] = e;
  }
  void pop() {
    if (!empty()) {
      --size_;
    }
    basilisk::term.fatal("can't pop() empty vector");
  }
  T top() const {
    if (!empty()) {
      return data_[size_];
    }
    basilisk::term.fatal("can't top() empty vector");
  }
  void clear() { size_ = 0; }

  class Iter {
   public:
    constexpr Iter(vector *v, size_t i = 0) : vector_{v}, index{i} {}
    void operator++() {
      if (index < vector_->size()) {
        index++;
      }
    }
    T &operator*() const { return (*vector_)[index]; }
    bool operator!=(const Iter &other) const {
      return other.vector_ != vector_ || other.index != index;
    }

   private:
    vector<T> *vector_;
    size_t index;
  };

  constexpr Iter begin() { return Iter{this}; }
  constexpr Iter end() { return Iter{this, size()}; }

 private:
  size_t capacity_, size_ = 0;
  T *data_ = nullptr;
};

template <class K, class V>
class map {
 public:
  explicit map(initializer_list<tuple<K, V>> list) : data_{list} {}
  void insert(K key, V value) { data_.push(tuple<K, V>{key, value}); }
  tuple<K, V> &operator[](K key) {
    for (auto &e : data_) {
      if (e.template get<0>() == key) {
        return e;
      }
    }
    basilisk::term.fatal("unable to find map key");
  }

 private:
  vector<tuple<K, V>> data_;
};

template <class Signature>
class function {
  function(Signature sig) : func_{sig} {}
  template <class Res, class... Args>
  Res operator()(Args... args) {
    return func_(args...);
  }

 private:
  Signature func_;
};

template <class Res, class Class, class... Args>
class function<Res (Class::*)(Args...)> {
 public:
  function(Res (Class::*f)(Args...)) : func_{f} {}
  Res operator()(Class *c, Args... args) { return c.*func_(args...); }
  Res operator()(const Class &c, Args... args) { return c.*func_(args...); }

 private:
  Res (Class::*func_)(Args...);
};

template <class Res, class... Args>
class function<Res(Args...)> {
 public:
  function(Res (*f)(Args...)) : func{f} {}
  Res operator()(Args... args) { return func(args...); }

 private:
  Res (*func)(Args...);
};

}  // namespace std

#endif  // SRC_LIBK_STD_H_
