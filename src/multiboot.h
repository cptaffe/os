// Copyright 2016 Connor Taffe

#ifndef SRC_MULTIBOOT_H_
#define SRC_MULTIBOOT_H_

#include <stdint.h>

namespace basilisk {

class alignas(4) Multiboot {
 public:
  constexpr Multiboot(uint32_t flags);
  constexpr Multiboot(uint32_t flags, uint32_t header_address,
                      uint32_t load_address, uint32_t load_end_address,
                      uint32_t bss_end_address, uint32_t entry_address,
                      uint32_t mode_type, uint32_t width, uint32_t height,
                      uint32_t depth);

  static const uint32_t kPageAlign = 1 << 0, kMemInfo = 1 << 1,
                        kVideoInfo = 1 << 2;

 private:
  uint32_t magic = 0x1badb002;
  uint32_t flags;
  [[gnu::unused]] uint32_t checksum, header_address, load_address,
      load_end_address, bss_end_address, entry_address, mode_type, width,
      height, depth;
};

constexpr Multiboot::Multiboot(uint32_t f, uint32_t ha, uint32_t la,
                               uint32_t lea, uint32_t bea, uint32_t ea,
                               uint32_t mt, uint32_t w, uint32_t h, uint32_t d)
    : flags{f},
      checksum{-(magic + flags)},
      header_address{ha},
      load_address{la},
      load_end_address{lea},
      bss_end_address{bea},
      entry_address{ea},
      mode_type{mt},
      width{w},
      height{h},
      depth{d} {}

constexpr Multiboot::Multiboot(uint32_t f)
    : Multiboot{f, 0, 0, 0, 0, 0, 0, 0, 0, 0} {}

}  // namespace basilisk

#endif  // SRC_MULTIBOOT_H_
