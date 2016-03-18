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

  basilisk::Kernel::getInstance()->halt();
}

void operator delete(void *mem) noexcept {
  if (heap != nullptr) {
    heap->deallocate(mem);
  }
}

namespace basilisk {

namespace {
Kernel kernel;
VGAScreen screen;
}

Kernel *Kernel::instance{&kernel};

Kernel *Kernel::getInstance() { return instance; }

void Kernel::onBoot() {
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

void Kernel::debug(const char *msg) {
  for (auto i = 0; msg[i]; i++) {
    screen.write(VGAScreen::Block{msg[i], VGAScreen::kRed, VGAScreen::kBlack});
  }
}

}  // namespace basilisk
