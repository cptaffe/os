// Copyright 2016 Connor Taffe

#include <stddef.h>
#include <stdint.h>

#include "src/libk/boot/multiboot/multiboot.h"
#include "src/libk/kernel.h"

extern "C" {
[[noreturn]][[gnu::unused]] extern void _start();
extern void constructors();
extern void deconstructors();
}

// Bootstrap stack used before we map a real stack
[[gnu::section(".bootstrap_stack")]] uint8_t bootstrap::stack[kStackSize];

namespace {
// Multiboot header required for some bootloaders
[[gnu::unused]][[gnu::section(".multiboot")]] basilisk::Multiboot header{
    basilisk::Multiboot::kPageAlign | basilisk::Multiboot::kMemInfo};
}  // namespace

// HACK: naked functions are unsupported by g++ on x86,
// but this function incidentally still works.
[[gnu::naked]] void _start() {
  asm("mov %0, %%esp\n"
      "call *%1\n" ::"g"(reinterpret_cast<intptr_t>(bootstrap::stack) +
                         sizeof(bootstrap::stack)),
      "g"(static_cast<void (*)()>([]() {
        constructors();
        basilisk::Kernel::instance()->onBoot();
      })));
}
