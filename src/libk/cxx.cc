// Copyright 2016 Connor Taffe

#include "src/libk/cxx.h"

#include "src/libk/ds/map.h"
#include "src/libk/kernel.h"
#include "src/libk/mem/heap.h"

basilisk::Heap *basilisk::heap{nullptr};

void *operator new(size_t size) {
  using basilisk::heap;
  if (heap != nullptr) {
    auto opt = heap->allocate<void>(size);
    if (opt.isOk()) {
      return opt.getValue();
    }
  }

  basilisk::Kernel::getInstance()->halt();
}

void *operator new[](size_t size) { return operator new(size); }

void operator delete(void *mem) noexcept {
  using basilisk::heap;
  if (heap != nullptr) {
    heap->deallocate(mem);
  }
}

void operator delete(void *mem, size_t) noexcept { operator delete(mem); }

void operator delete[](void *mem) noexcept { return operator delete(mem); }

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
