// Copyright 2016 Connor Taffe

#include "src/libk/kernel.h"

namespace {
const char *version =
    "Basilisk Kernel version 0.1.\nCopyright (c) 2016 Connor Taffe. All "
    "rights reserved.\n";
}  // namespace

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
      buffer[buffer_index] = str[i] | 9 << 8;
      advance();
    }
  }
  newline();
}

Terminal term{reinterpret_cast<uint16_t *>(static_cast<intptr_t>(0xb8000)), 25,
              80};

class Heap {
 public:
  Heap() { *reinterpret_cast<Header *>(buffer) = Header{sizeof(buffer)}; }
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
      return nullptr;
    }
  }

  void dealloc(void *p) {
    for (int i = 0; i < sizeof(free_list) / sizeof(Header *); i++) {
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
  uint8_t buffer[4096];
  Header *free_list[10];
};

Heap heap;

}  // namespace

void *operator new(size_t s) { return heap.alloc(s); }

void operator delete(void *p) { heap.dealloc(p); }

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
  asm("cli\n"
      "hlt\n");  // never returns
}

}  // namespace basilisk
