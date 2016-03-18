// Copyright 2016 Connor Taffe

#include "src/vga.h"

namespace basilisk {

uint16_t VGAScreen::Block::getBlock() const { return block; }

void VGAScreen::write(Block b) {
  i++;
  column = i % width;
  line = (i / width) % height;
  buffer[i % (width * height)] = b;
}

}  // namespace basilisk
