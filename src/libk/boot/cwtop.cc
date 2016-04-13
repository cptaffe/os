// Copyright 2016 Connor Taffe

asm(".section .init\n"
    ".globl constructors\n"
    "constructors:\n"
    "push %ebp\n"
    "movl %esp, %ebp\n");
// crtbegin.o .init section here

asm(".section .fini\n"
    ".globl deconstructors\n"
    "deconstructors:\n"
    "push %ebp\n"
    "movl %esp, %ebp\n");
// crtbegin.o .fini section here.
