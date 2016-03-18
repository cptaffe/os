// Copyright 2016 Connor Taffe

#ifndef SRC_VGA_H_
#define SRC_VGA_H_

#include "src/types.h"

namespace basilisk {

class VGAScreen {
 public:
  enum : uint8_t {
    kBlack,
    kBlue,
    kGreen,
    kCyan,
    kRed,
    kMagenta,
    kBrown,
    kGrey,
    kBright = 1 << 4
  };

  class Block {
   public:
    constexpr Block(char character, uint8_t foreground, uint8_t background);
    uint16_t getBlock() const;

   private:
    uint16_t block;
  };

  void write(Block blocks);

 private:
  Block *buffer = reinterpret_cast<Block *>(static_cast<intptr_t>(0xb8000));
  size_t height = 25, width = 80;
  size_t column = 0, line = 0, i = 0;
};

constexpr VGAScreen::Block::Block(char character, uint8_t foreground,
                                  uint8_t background)
    : block{static_cast<uint16_t>(character |
                                  (foreground | background << 4) << 8)} {}

}  // namespace basilisk

#endif  // SRC_VGA_H_
