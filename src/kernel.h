// Copyright 2016 Connor Taffe

#ifndef SRC_KERNEL_H_
#define SRC_KERNEL_H_

#include <stddef.h>
#include <stdint.h>

#include "src/vga.h"

namespace basilisk {

class Terminal {
 public:
  virtual ~Terminal() {}
  virtual void write(const char c) = 0;
  virtual size_t getX() const = 0;
  virtual void setX(const size_t x) = 0;
  virtual size_t getY() const = 0;
  virtual void setY(const size_t y) = 0;
  virtual size_t getHeight() const = 0;
  virtual size_t getWidth() const = 0;
};

class Stream : public Terminal {
 public:
  explicit constexpr Stream(VGAScreen *screen);

  const VGAScreen::Color &getForegroundColor() const { return foreground; }
  void setForegroundColor(const VGAScreen::Color &color) { foreground = color; }
  const VGAScreen::Color &getBackgroundColor() const { return background; }
  void setBackgroundColor(const VGAScreen::Color &color) { background = color; }
  void clear();

  void write(const char c) override;
  size_t getX() const override { return j; }
  void setX(const size_t x) override { j = x; }
  size_t getY() const override { return i; }
  void setY(const size_t y) override { i = y; }
  size_t getHeight() const override { return screen->getHeight(); }
  size_t getWidth() const override { return screen->getWidth(); }

  friend Stream &operator<<(Stream &, const char *);

 private:
  VGAScreen *screen;
  VGAScreen::Color foreground = VGAScreen::kWhite,
                   background = VGAScreen::kBlack;
  size_t i = 0, j = 0;
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

#endif  // SRC_KERNEL_H_
