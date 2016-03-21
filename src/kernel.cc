// Copyright 2016 Connor Taffe

#include "src/kernel.h"

#include "src/heap.h"

namespace {
basilisk::Heap *heap{nullptr};
const char *version =
    "Basilisk Kernel version 0.1.\nCopyright (c) 2016 Connor Taffe. All "
    "rights reserved.\n";
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

void *operator new[](size_t size) { return operator new(size); }

void operator delete(void *mem) noexcept {
  if (heap != nullptr) {
    heap->deallocate(mem);
  }
}

void operator delete(void *mem, size_t) noexcept { operator delete(mem); }

void operator delete[](void *mem) noexcept { return operator delete(mem); }

namespace basilisk {

namespace {
VGAScreen screen;
Stream stream{&screen};
Kernel kernel;
}

Kernel *Kernel::instance{&kernel};
Stream *Kernel::debug_stream{&stream};

Kernel *Kernel::getInstance() { return instance; }

void Kernel::onBoot() {
  getDebugStream().clear();
  getDebugStream() << version;

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

Stream &Kernel::getDebugStream() { return *debug_stream; }

void Stream::clear() {
  VGAScreen::Block b{' ', foreground, background};
  for (i = 0; i < screen->getHeight(); i++) {
    for (j = 0; j < screen->getWidth(); j++) {
      screen->plot(b, i, j);
    }
  }
  i = j = 0;
}

class Printer {
 public:
  Printer(Terminal *screen, size_t tab_width);
  void print(const char *str);

 private:
  size_t tab_width;
  Terminal *screen;
};

Printer::Printer(Terminal *s, size_t tw) : tab_width(tw), screen(s) {}

void Printer::print(const char *str) {
  for (size_t i = 0; str[i]; i++) {
    switch (str[i]) {
      case '\t':
        // tab length of 4 spaces
        {
          auto n = tab_width - screen->getX() % tab_width;
          for (size_t j = 0; j < n; j++) {
            screen->write(' ');
          }
        }
        break;
      case '\n':
        screen->setY(screen->getY() + 1 % screen->getHeight());
        [[fallthrough]];
      case '\r':
        screen->setX(0);
        break;
      default:
        screen->write(str[i]);
    }
  }
}

void Stream::write(const char c) {
  screen->plot(VGAScreen::Block{c, foreground, background}, i, j);
  j++;
  if (j > screen->getWidth()) {
    j = 0;
    i++;
    if (i > screen->getHeight()) {
      for (size_t k = 1; k < screen->getHeight(); k++) {
        for (size_t l = 0; l < screen->getWidth(); l++) {
          screen->plot(screen->read(k, l), k - 1, l);
        }
      }
      i--;
    }
  }
}

Stream &operator<<(Stream &s, const char *msg) {
  Printer(&s, 4).print(msg);
  return s;
}

}  // namespace basilisk
