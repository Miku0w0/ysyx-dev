#include "npc.h"
#include <iostream>

// 模拟 128MB 内存
#define MEM_SIZE 0x8000000
#define MEM_BASE 0x80000000

static uint8_t mem[MEM_SIZE];

// 将虚拟地址转换为模拟内存数组的索引
static inline uint8_t *guest_to_host(uint32_t paddr) {
  return mem + (paddr - MEM_BASE);
}

extern "C" int pmem_read(int raddr) {
  // 简易边界检查
  if (raddr < MEM_BASE || raddr >= MEM_BASE + MEM_SIZE)
    return 0;

  // 总是返回 4 字节对齐的数据
  return *(int *)guest_to_host(raddr & ~0x3u);
}

extern "C" void pmem_write(int waddr, int wdata, char wmask) {
  if (waddr < MEM_BASE || waddr >= MEM_BASE + MEM_SIZE)
    return;

  uint8_t *p = guest_to_host(waddr & ~0x3u);
  // 根据 wmask 按字节写入
  for (int i = 0; i < 4; i++) {
    if ((wmask >> i) & 1) {
      p[i] = (uint8_t)((wdata >> (i * 8)) & 0xFF);
    }
  }
}

// 供 main 调用：把编译好的二进制文件读入模拟内存
void load_img(char *img_file) {
  if (img_file == NULL)
    return;
  FILE *fp = fopen(img_file, "rb");
  if (fp == NULL) {
    printf("Can't open %s\n", img_file);
    return;
  }
  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  int ret = fread(guest_to_host(MEM_BASE), size, 1, fp);
  fclose(fp);
}