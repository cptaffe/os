// Copyright 2016 Connor Taffe

#ifndef SRC_KERNEL_H_
#define SRC_KERNEL_H_

#include "src/types.h"

namespace basilisk {

class Kernel {
 public:
  static Kernel *getInstance();
  [[noreturn]] void onBoot();
  [[noreturn]] void halt();

 private:
  static Kernel *instance;
};

}  // namespace basilisk

void *operator new(size_t size);
void operator delete(void *mem) noexcept;

#endif  // SRC_KERNEL_H_
