// Copyright 2016 Connor Taffe

// crtbegin.o .init section here
asm(".section .init\n"
    "popl %ebp\n"
    "ret\n");

// crtbegin.o .fini section here.
asm(".section .fini\n"
    "popl %ebp\n"
    "ret\n");
