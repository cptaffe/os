// Copyright 2016 Connor Taffe

#include <stddef.h>
#include <stdint.h>

#include "src/kernel.h"
#include "src/multiboot.h"
#include "src/result.h"

extern "C" {
[[noreturn]] void _start();
[[noreturn]] void kstart();
extern uint8_t stack[];
}

namespace {
// boot images
[[gnu::unused]][[gnu::section(".multiboot")]] basilisk::Multiboot header{
    basilisk::Multiboot::kPageAlign | basilisk::Multiboot::kMemInfo |
    basilisk::Multiboot::kVideoInfo};
}  // namespace

[[gnu::unused]][[gnu::section(".bootstrap_stack")]] uint8_t stack[16384];

// [[gnu::naked]] void _start() {
//   asm("movl %0, %%esp\n"
//       "call kstart\n" ::"g"(reinterpret_cast<intptr_t>(stack) +
//       sizeof(stack)));
// }

asm(".globl _start\n"
    "_start:\n"
    "movl $stack+16384, %esp\n"
    "call kstart\n");

void kstart() {
  *reinterpret_cast<uint16_t*>(static_cast<intptr_t>(0xb8000)) = 'A' | 4 << 8;
  basilisk::Kernel::getInstance()->onBoot();
}
