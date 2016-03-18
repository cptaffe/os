# os
Silly bootable program

```sh
$ clang++ -ffreestanding -fno-exceptions -fno-rtti -nostdlib -Wall -Wextra -I. --std=c++1z src/boot.cc src/heap.cc src/kernel.cc src/result.cc src/vga.cc -T src/link.ld -o k.o --target=i686-pc-none-elf -march=i686 -L${HOME}/opt/cross/lib -g
```
