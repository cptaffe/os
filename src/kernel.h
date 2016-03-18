// Copyright 2016 Connor Taffe

#ifndef SRC_KERNEL_H_
#define SRC_KERNEL_H_

#include <stddef.h>
#include <stdint.h>

namespace basilisk {

class Kernel {
 public:
  static Kernel *getInstance();
  [[noreturn]] void onBoot();
  [[noreturn]] void halt();
  void debug(const char *msg);

 private:
  static Kernel *instance;
};

}  // namespace basilisk

void *operator new(size_t size);
void operator delete(void *mem) noexcept;

#endif  // SRC_KERNEL_H_
