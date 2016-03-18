// Copyright 2016 Connor Taffe

#include "src/kernel.h"

#include "src/heap.h"
#include "src/vga.h"

namespace {
basilisk::Heap *heap{nullptr};
}

void *operator new(size_t size) {
  if (heap != nullptr) {
    auto opt = heap->allocate<void>(size);
    if (opt.isOk()) {
      return opt.getValue();
    }
  }
  return nullptr;
}

void operator delete(void *mem) noexcept {
  if (heap != nullptr) {
    heap->deallocate(mem);
  }
}

namespace basilisk {

namespace {
Kernel kernel;
}

Kernel *Kernel::instance{&kernel};

Kernel *Kernel::getInstance() { return instance; }

void Kernel::onBoot() {
  VGAScreen screen;
  screen.write(VGAScreen::Block{'K', VGAScreen::kRed, VGAScreen::kBlack});

  // allocate stack space for heap
  uint8_t heap_buffer[1024];
  auto hp = Heap{heap_buffer, sizeof(heap_buffer)};
  heap = &hp;

  halt();
}

void Kernel::halt() {
  // turn off interrrupts and halt
  asm("cli\n"
      "hlt\n");

  for (;;) {
    // loop forever
  }
}

}  // namespace basilisk
