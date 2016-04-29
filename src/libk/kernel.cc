// Copyright 2016 Connor Taffe

#include "src/libk/kernel.h"
#include "src/libk/std.h"

extern "C" {
void *memset(void *s, int c, size_t n);
}  // extern "C"

void *memset(void *s, int c, size_t n) {
  auto src = static_cast<uint8_t *>(s);
  uint8_t value = static_cast<uint8_t>(c);
  for (size_t i = 0; i < n; i++) {
    src[i] = value;
  }
  return src;
}

namespace {

const char *version =
    "Basilisk Kernel version 0.1.\nCopyright (c) 2016 Connor Taffe. All "
    "rights reserved.\n";

// halts and never returns
[[noreturn]] void halt() {
  asm __attribute__((noreturn)) (
      "cli\n"
      "hlt\n");
}

class Heap {
 public:
  Heap() { new (data_.data()) Header{sizeof(data_)}; }
  void *alloc(size_t s) {
    s = s + sizeof(uint64_t) % sizeof(uint64_t);  // quadword aligned
    if (free_list[0] != nullptr) {
      // still have free entries
      if (free_list[0]->size() > s) {
        Header *h = free_list[0];
        // TODO(cptaffe): try to split
        free_list[0] = h->split(s);
        return h->data();
      }
    }
    // impossible to allocate more memory
    // TODO(cptaffe): implement exceptions, catch in onBoot()
    basilisk::term.println("out of memory");
    halt();
  }

  void dealloc(void *p) {
    for (auto *e : free_list) {
      if (*e == nullptr) {
        *e = Header::fromAddress(p);
      }
    }
  }

  class Header {
    friend class Heap;

   public:
    explicit Header(size_t s) : size_{s} {}
    size_t size() const { return size_; }
    void *data() {
      return &(reinterpret_cast<uint8_t *>(this)[sizeof(Header)]);
    }
    static Header *fromAddress(void *p) {
      return reinterpret_cast<Header *>(
          &(static_cast<uint8_t *>(p)[-sizeof(Header)]));
    }

   private:
    size_t size_;

    // split returns a new Header with excess quadword aligned bytes,
    // or nullptr
    Header *split(size_t s) {
      // at least a quadword
      if (size_ - (sizeof(Header) + s) >= (sizeof(Header) + sizeof(uint64_t))) {
        this[sizeof(Header) + s] = Header{size_ - (s + 2 * sizeof(Header))};
        size_ = s;
        return &this[sizeof(Header) + size_];
      }
      return nullptr;
    }
  };

 private:
  std::array<uint8_t, 4096> data_;
  std::array<Header *, 10> free_list =
      std::array<Header *, 10>{reinterpret_cast<Header *>(data_.data())};
};

Heap heap;

}  // namespace

void *operator new(size_t s) { return heap.alloc(s); }
void *operator new[](size_t s) { return operator new(s); }

void operator delete(void *p) noexcept { heap.dealloc(p); }
void operator delete[](void *p) { return operator delete(p); }

namespace basilisk {

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

namespace interrupts {

namespace {

// HACK: naked functions are unsupported by g++ on x86,
// but this function incidentally still works.
// NOTE: runs kernel on separate interrupt stack.
[[gnu::naked]] void interrupt() {
  asm("movq %0, %%rsp\n"
      "callq *%1\n" ::"g"(reinterpret_cast<intptr_t>(bootstrap::stack) +
                          sizeof(bootstrap::stack)),
      "g"(static_cast<void (*)()>(
          []() { basilisk::Kernel::instance()->onInterrupt(); })));
}

}  // namespace

class Idt {
 public:
  Idt(uint32_t offset, uint16_t s, uint8_t ta)
      : offset_1{static_cast<uint16_t>(offset & 0xffff)},
        selector{s},
        type_attr{ta},
        offset_2{static_cast<uint16_t>((offset & 0x0000ffff) << 8)} {}

 private:
  [[gnu::unused]] uint16_t offset_1;  // offset bits 0..15
  [[gnu::unused]] uint16_t selector;  // a code segment selector in GDT or LDT
  [[gnu::unused]] uint8_t zero = 0;   // unused, set to 0
  [[gnu::unused]] uint8_t type_attr;  // type and attributes, see below
  [[gnu::unused]] uint16_t offset_2;  // offset bits 16..31
};

}  // namespace interrupts

Kernel *Kernel::instance_ = nullptr;

Kernel::Kernel() { term.println("kernel constructor running"); }

Kernel *Kernel::instance() {
  if (instance_ == nullptr) {
    instance_ = new Kernel{};
  }
  return instance_;
}

template <class T>
void test(std::function<T> f) {
  f();
}

void Kernel::onBoot() {
  term.println("onBoot: here");
  term.println(version);
  auto m = std::map<int, int>{std::tuple<int, int>{1, 2},
                              std::tuple<int, int>{3, 4}};
  if (m[1].template get<1>() == 2) {
    term.println("hello");
  }
  onInterrupt();
  term.fatal("halting...");
}

void Kernel::onInterrupt() { term.fatal("onInterrupt: here"); }

}  // namespace basilisk
