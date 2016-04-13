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

namespace {
// Multiboot header required for some bootloaders
[[gnu::unused]][[gnu::section(".multiboot")]] basilisk::Multiboot header{
    basilisk::Multiboot::kPageAlign | basilisk::Multiboot::kMemInfo};
// Bootstrap stack used before we map a real stack
[[gnu::section(".bootstrap_stack")]] uint8_t stack[16384];
}  // namespace

// HACK: naked functions are unsupported by g++ on x86,
// but this function incidentally still works.
[[gnu::naked]] void _start() {
  asm("movl %0, %%esp\n"
      "calll *%1\n" ::"g"(reinterpret_cast<intptr_t>(stack) + sizeof(stack)),
      "g"(static_cast<void (*)()>([]() {
        constructors();
        basilisk::Kernel::instance()->onBoot();
      })));
}
