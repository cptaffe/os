# os
Silly bootable program

Use the following command to compile a bootable elf. Have a cross compiled binutils for i686-elf in your path and replace `${HOME}/opt/corss/lib` with the path to that lib.

```sh
$ i686-elf-g++ -ffreestanding -fno-exceptions -fno-rtti -nostdlib -Wall -Wextra -I. --std=c++1z src/boot.cc src/heap.cc src/kernel.cc src/result.cc src/vga.cc -T src/link.ld -o k.o -L${HOME}/opt/cross/lib -g
```
