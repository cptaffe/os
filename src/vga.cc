// Copyright 2016 Connor Taffe

#include "src/vga.h"

namespace basilisk {

void VGAScreen::plot(Block b, size_t i, size_t j) { buffer[i * width + j] = b; }
VGAScreen::Block VGAScreen::read(size_t i, size_t j) {
  return buffer[i * width + j];
}

}  // namespace basilisk
