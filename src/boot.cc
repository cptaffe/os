// Copyright 2016 Connor Taffe

#include <stddef.h>
#include <stdint.h>

#include "src/kernel.h"
#include "src/multiboot.h"
#include "src/result.h"

extern "C" {
[[noreturn]] void _start();
}

namespace {
// boot images
[[gnu::unused]][[gnu::section(".multiboot")]] basilisk::Multiboot header{
    basilisk::Multiboot::kPageAlign | basilisk::Multiboot::kMemInfo |
    basilisk::Multiboot::kVideoInfo};
[[gnu::section(".bootstrap_stack")]] uint8_t stack[16384];
}  // namespace

// HACK: naked functions are unsupported by g++ on x86,
// but this function incidentally still works.
[[gnu::naked]] void _start() {
  asm("movl %0, %%esp\n"
      "calll %1\n" ::"g"(reinterpret_cast<intptr_t>(stack) + sizeof(stack)),
      "g"(static_cast<void (*)()>(
          [] { basilisk::Kernel::getInstance()->onBoot(); })));
}
