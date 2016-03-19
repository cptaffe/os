// Copyright 2016 Connor Taffe

#ifndef SRC_KERNEL_H_
#define SRC_KERNEL_H_

#include <stddef.h>
#include <stdint.h>

#include "src/vga.h"

namespace basilisk {

class Stream {
 public:
  explicit constexpr Stream(VGAScreen *screen);

  VGAScreen::Color getForegroundColor() const { return foreground; }
  void setForegroundColor(const VGAScreen::Color &color) { foreground = color; }
  VGAScreen::Color getBackgroundColor() const { return background; }
  void setBackgroundColor(const VGAScreen::Color &color) { background = color; }

  friend Stream &operator<<(Stream &, const char *);

 private:
  VGAScreen *screen;
  VGAScreen::Color foreground = VGAScreen::kRed, background = VGAScreen::kBlack;
};

constexpr Stream::Stream(VGAScreen *sc) : screen{sc} {}

class Kernel {
 public:
  static Kernel *getInstance();
  [[noreturn]] void onBoot();
  [[noreturn]] void halt();
  Stream &getDebugStream();

 private:
  static Stream *debug_stream;
  static Kernel *instance;
};

}  // namespace basilisk

void *operator new(size_t size);
void operator delete(void *mem) noexcept;

#endif  // SRC_KERNEL_H_
