// Copyright 2016 Connor Taffe

#include <stddef.h>
#include <stdint.h>

#include "src/kernel.h"
#include "src/map.h"
#include "src/multiboot.h"
#include "src/result.h"

extern "C" {
[[noreturn]][[gnu::unused]] void _start();
}

namespace {
// Multiboot header required for some bootloaders
[[gnu::unused]][[gnu::section(".multiboot")]] basilisk::Multiboot header{
    basilisk::Multiboot::kPageAlign | basilisk::Multiboot::kMemInfo};
// Bootstrap stack used before we map a real stack
[[gnu::section(".bootstrap_stack")]] uint8_t stack[16384];
}  // namespace

// HACK: naked functions are unsupported by g++ on x86,
// but this function incidentally still works.
[[gnu::naked]] void _start() {
  asm("movl %0, %%esp\n"
      "calll *%1\n" ::"g"(reinterpret_cast<intptr_t>(stack) + sizeof(stack)),
      "g"(static_cast<void (*)()>(
          []() { basilisk::Kernel::getInstance()->onBoot(); })));
}

extern "C" {
void __cxa_pure_virtual();
int __cxa_atexit(void (*destructor)(void *), void *arg, void *dso_handle);
void __cxa_finalize(void *func);
extern void *__dso_handle;
}

void *__dso_handle = nullptr;

static void print(const char *str) {
  basilisk::Kernel::getInstance()->getDebugStream() << str;
}

void __cxa_pure_virtual() {
  static bool may_recurse = false;
  if (may_recurse) {
    return;
  }
  // set may_recurse because printing
  // is complex and may result in a recursion into this function.
  may_recurse = true;
  print("Pure virtual method called\n");
  may_recurse = false;
  return;
}

class ABIDestructor {
 public:
  ABIDestructor(void *object, void (*destructor)(void *), void *handle);
  void (*getDestructor() const)(void *) { return destructor; }
  void run();

  static basilisk::Map<void (*)(void *), ABIDestructor> destructors;

 private:
  void *object;
  void (*destructor)(void *);
  void *handle;
};

basilisk::Map<void (*)(void *), ABIDestructor> ABIDestructor::destructors;

ABIDestructor::ABIDestructor(void *o, void (*d)(void *), void *h)
    : object{o}, destructor{d}, handle{h} {}

void ABIDestructor::run() { destructor(object); }

int __cxa_atexit(void (*destructor)(void *), void *object, void *handle) {
  print("registering destructor");
  ABIDestructor::destructors.insert(destructor,
                                    ABIDestructor{object, destructor, handle});
  return 0;
}

void __cxa_finalize(void *func) {
  print("calling destructor");
  auto rm = [](ABIDestructor d) {
    d.run();
    ABIDestructor::destructors.erase(d.getDestructor());
  };

  if (func == nullptr) {
    for (auto d : ABIDestructor::destructors) {
      rm(d.getValue());
    }
  } else {
    rm(ABIDestructor::destructors[reinterpret_cast<void (*)(void *)>(func)]);
  }
}
