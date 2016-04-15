// Copyright 2016 Connor Taffe

#include "src/libk/kernel.h"

namespace {
const char *version =
    "Basilisk Kernel version 0.1.\nCopyright (c) 2016 Connor Taffe. All "
    "rights reserved.\n";
}  // namespace

namespace __gnu_pbds {
namespace detail {}  // namespace detail
}  // namespace __gnu_pbds

namespace std {

template <typename T>
class initializer_list {
 public:
};

template <typename T, size_t S>
class array {
 public:
  explicit array(std::intializer_list<T> b) : buffer{b} {}
  constexpr size_t size() const { return S; }
  constexpr bool empty() const { return size() == 0; }
  constexpr T &operator[](size_t i) { return buffer[i]; }
  constexpr T *data() { return buffer; }
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
    constexpr Iter(array *a, size_t i) : array{a}, index{i} {}
    void operator++() {
      if (index < array->size() - 1) {
        index++;
      }
    }
    T *operator*() const { return &array[index]; }
    bool operator!=(const Iter &other) const {
      return other.array != array || other.index != index;
    }

   private:
    array<T, S> *array;
    size_t index;
  };

  constexpr Iter begin() { return Iter{this, 0}; }
  constexpr Iter end() { return Iter{this, size() - 1}; }

 private:
  T buffer[S];
};
}  // namespace std

namespace {

class Terminal {
 public:
  Terminal(uint16_t *buf, size_t h, size_t w)
      : buffer(buf), height(h), width(w) {}
  void println(const char *str);

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

// NOTE: term is used in Heap, so it must not allocate with 'new'
Terminal term{reinterpret_cast<uint16_t *>(static_cast<intptr_t>(0xb8000)), 25,
              80};

// halts and never returns
[[noreturn]] void halt() {
  asm("cli\n"
      "hlt\n");
}

class Heap {
 public:
  Heap() {
    *reinterpret_cast<Header *>(buffer.data()) = Header{sizeof(buffer)};
  }
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
  std::array<uint8_t, 4096> buffer;
  std::array<Header *, 10> free_list = {
      {reinterpret_cast<Header *>(buffer.data())}};
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
  halt();
}

void Kernel::halt() {
  term.println("halting...");
  // turn off interrrupts and halt
  ::halt();
}

}  // namespace basilisk
