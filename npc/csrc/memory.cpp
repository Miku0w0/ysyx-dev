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

// 地址转换函数：将低地址自动映射到 MEM_BASE
static inline uint32_t translate_addr(uint32_t addr) {
  if (addr != 0 && addr < MEM_BASE) {
    return addr + MEM_BASE;
  }
  return addr;
}

extern "C" int pmem_read(uint32_t raddr) {
  // 地址转换：将低地址映射到 MEM_BASE
  uint32_t paddr = translate_addr(raddr);
  // 简易边界检查
  if (paddr == 0) return 0;
  if (paddr < MEM_BASE || paddr >= MEM_BASE + MEM_SIZE) {
    return 0;
  }
  if (raddr == 0x80000034) {
    printf("[MEM] srai instruction at 0x%x: %02x %02x %02x %02x\n", raddr,
           mem[raddr - MEM_BASE], mem[raddr - MEM_BASE + 1],
           mem[raddr - MEM_BASE + 2], mem[raddr - MEM_BASE + 3]);
  }
  // 总是返回 4 字节对齐的数据
  uint8_t *host_ptr = guest_to_host(paddr & ~0x3u);
  int data = *(int *)host_ptr;
  return data;
}

extern "C" void pmem_write(int waddr, int wdata, char wmask) {
  // 地址转换：将低地址映射到 MEM_BASE
  uint32_t paddr = translate_addr(waddr);
  if (paddr < MEM_BASE || paddr >= MEM_BASE + MEM_SIZE)
    return;

  uint8_t *p = guest_to_host(paddr & ~0x3u);
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

  printf("[NPC] Image file size: %ld bytes\n", size);
  if (size <= 0) {
    printf("[NPC] Error: Image size is 0!\n");
    exit(1);
  }

  uint8_t *host_addr = guest_to_host(MEM_BASE);
  printf("[NPC] Destination host address: %p\n", host_addr);

  size_t ret = fread(host_addr, size, 1, fp);
  if (ret != 1) {
    printf("[NPC] Error: fread failed!\n");
  }

  fclose(fp);
  printf("[NPC] Load success!\n");
}