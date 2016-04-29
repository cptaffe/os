// Copyright 2016 Connor Taffe

#ifndef SRC_LIBK_KERNEL_H_
#define SRC_LIBK_KERNEL_H_

#include <stddef.h>
#include <stdint.h>

namespace bootstrap {

enum : size_t { kStackSize = 16384 };
extern uint8_t stack[kStackSize];

}  // namespace bootstrap

namespace basilisk {

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

extern Terminal term;

class Kernel {
 public:
  static Kernel *instance();
  [[noreturn]] void onBoot();
  void onInterrupt();

 private:
  Kernel();
  static Kernel *instance_;
};

}  // namespace basilisk

#endif  // SRC_LIBK_KERNEL_H_
