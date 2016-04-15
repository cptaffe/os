// Copyright 2016 Connor Taffe

#include "src/libk/kernel.h"

namespace {
const char *version =
    "Basilisk Kernel version 0.1.\nCopyright (c) 2016 Connor Taffe. All "
    "rights reserved.\n";
}  // namespace

namespace std {

template <typename T>
class initializer_list {
 public:
  constexpr initializer_list(const T *b, size_t s) : data_{b}, size_{s} {}
  constexpr size_t size() { return size_; }
  constexpr T operator[](size_t i) { return data_[i]; }

  class Iter {
   public:
    constexpr Iter(initializer_list *a, size_t i) : list_{a}, index{i} {}
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

  constexpr Iter begin() { return Iter{this, 0}; }
  constexpr Iter end() { return Iter{this, size() - 1}; }

 private:
  const T *data_;
  size_t size_;
};

template <typename T, size_t S>
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
    constexpr Iter(array *a, size_t i) : array_{a}, index{i} {}
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

  constexpr Iter begin() { return Iter{this, 0}; }
  constexpr Iter end() { return Iter{this, size() - 1}; }

 private:
  T data_[S];
};

}  // namespace std

namespace {

// halts and never returns
[[noreturn]] void halt() {
  asm("cli\n"
      "hlt\n");
}

class Terminal {
 public:
  Terminal(uint16_t *buf, size_t h, size_t w)
      : buffer(buf), height(h), width(w) {}
  void println(const char *str);
  [[noreturn]] void fatal(const char *str);

 private:
  size_t buffer_index = 0;
  uint16_t *buffer;
  size_t height, width;

  void newline();
  void advance();
};

void Terminal::newline() {
  buffer_index = (buffer_index + width) / width * width;
}

void Terminal::advance() { buffer_index++; }

void Terminal::println(const char *str) {
  for (size_t i = 0; str[i]; i++) {
    if (str[i] == '\n') {
      newline();
    } else {
      buffer[buffer_index] = static_cast<uint16_t>(str[i] | 9 << 8);
      advance();
    }
  }
  newline();
}

void Terminal::fatal(const char *str) {
  println(str);
  halt();
}

// NOTE: term is used in Heap, so it must not allocate with 'new'
Terminal term{reinterpret_cast<uint16_t *>(static_cast<intptr_t>(0xb8000)), 25,
              80};

class Heap {
 public:
  Heap() { *reinterpret_cast<Header *>(data_.data()) = Header{sizeof(data_)}; }
  void *alloc(size_t s) {
    term.println("allocating");
    if (free_list[0] != nullptr) {
      // still have free entries
      if (free_list[0]->size() > s) {
        Header *h = free_list[0];
        // TODO(cptaffe): try to split
        free_list[0] = nullptr;
        return h->toAddress();
      }
    }
    // impossible to allocate more memory
    // TODO(cptaffe): implement exceptions, catch in onBoot()
    term.println("fatal: out of memory");
    halt();
  }

  void dealloc(void *p) {
    for (size_t i = 0; i < sizeof(free_list) / sizeof(Header *); i++) {
      if (free_list[i] == nullptr) {
        free_list[i] = Header::fromAddress(p);
      }
    }
  }

  class Header {
   public:
    explicit Header(size_t s) : size_{s} {}
    size_t size() const { return size_; }
    void *toAddress() {
      return &(reinterpret_cast<uint8_t *>(this)[sizeof(Header)]);
    }
    static Header *fromAddress(void *p) {
      return reinterpret_cast<Header *>(
          &(static_cast<uint8_t *>(p)[-sizeof(Header)]));
    }

   private:
    size_t size_;
  };

 private:
  std::array<uint8_t, 4096> data_;
  std::array<Header *, 10> free_list =
      std::array<Header *, 10>{reinterpret_cast<Header *>(data_.data())};
};

Heap heap;

}  // namespace

void *operator new(size_t s) { return heap.alloc(s); }

void operator delete(void *p) noexcept { heap.dealloc(p); }

namespace basilisk {

Kernel *Kernel::instance_ = nullptr;

Kernel::Kernel() { term.println("kernel constructor running"); }

Kernel *Kernel::instance() {
  if (instance_ == nullptr) {
    instance_ = new Kernel{};
  }
  return instance_;
}

void Kernel::onBoot() {
  term.println("onBoot: here");
  term.println(version);
  term.fatal("halting...");
}

}  // namespace basilisk
