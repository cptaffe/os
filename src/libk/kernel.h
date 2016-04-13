// Copyright 2016 Connor Taffe

#ifndef SRC_LIBK_KERNEL_H_
#define SRC_LIBK_KERNEL_H_

#include <stddef.h>
#include <stdint.h>

namespace basilisk {

class Kernel {
 public:
  static Kernel *instance();
  [[noreturn]] void onBoot();
  [[noreturn]] void halt();

 private:
  Kernel();
  static Kernel *instance_;
};

}  // namespace basilisk

#endif  // SRC_LIBK_KERNEL_H_
