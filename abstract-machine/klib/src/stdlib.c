#include <am.h>
#include <klib.h>
#include <klib-macros.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
static unsigned long int next = 1;
static char *h_ptr = NULL;

int rand(void) {
  // RAND_MAX assumed to be 32767
  next = next * 1103515245 + 12345;
  return (unsigned int)(next/65536) % 32768;
}

void srand(unsigned int seed) {
  next = seed;
}

int abs(int x) {
  return (x < 0 ? -x : x);
}

int atoi(const char* nptr) {
  int x = 0;
  while (*nptr == ' ') { nptr ++; }
  while (*nptr >= '0' && *nptr <= '9') {
    x = x * 10 + *nptr - '0';
    nptr ++;
  }
  return x;
}

void *malloc(size_t size) {
  // On native, malloc() will be called during initializaion of C runtime.
  // Therefore do not call panic() here, else it will yield a dead recursion:
  //   panic() -> putchar() -> (glibc) -> malloc() -> panic()

#if !(defined(__ISA_NATIVE__) && defined(__NATIVE_USE_KLIB__))
  // 初始化：第一次调用时，将指针指向堆区的起点
  if (h_ptr == NULL) {
    h_ptr = (char *)heap.start;
  }

  // 对齐：为了性能和兼容性，通常将分配大小向上对齐到 8 字节
  size = (size + 7) & ~7;

  // 记录当前分配的起始地址
  void *ret = h_ptr;

  // 移动堆指针，预留出 size 字节的空间
  h_ptr += size;

  // 越界检查：如果超过了 heap.end，说明内存耗尽
  if ((uintptr_t)h_ptr > (uintptr_t)heap.end) {
    return NULL;
  }
  return ret;
#endif
  return NULL;
}

void free(void *ptr) {
}

#endif
