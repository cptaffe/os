// Copyright 2016 Connor Taffe

#ifndef SRC_VGA_H_
#define SRC_VGA_H_

#include <stddef.h>
#include <stdint.h>

namespace basilisk {

class VGAScreen {
 public:
  enum Color : uint8_t {
    kBlack,
    kBlue,
    kGreen,
    kCyan,
    kRed,
    kMagenta,
    kBrown,
    kLightGray,
    kDarkGray,
    kLightBlue,
    kLightGreen,
    kLightCyan,
    kLightRed,
    kLightMagenta,
    kYello,
    kWhite,
  };

  class Block {
   public:
    constexpr Block(char character, uint8_t foreground, uint8_t background);

   private:
    [[gnu::unused]] uint16_t block;
  };

  size_t getWidth() const { return width; }
  size_t getHeight() const { return height; }
  void plot(Block block, size_t x, size_t y);
  Block read(size_t x, size_t y);

 private:
  Block *buffer = reinterpret_cast<Block *>(static_cast<intptr_t>(0xb8000));
  const size_t height = 25, width = 80;
};

constexpr VGAScreen::Block::Block(char character, uint8_t foreground,
                                  uint8_t background)
    : block{static_cast<uint16_t>(character |
                                  (foreground | background << 4) << 8)} {}

}  // namespace basilisk

#endif  // SRC_VGA_H_
