#include "../npc.h"

extern uint8_t *guest_to_host(uint32_t paddr);
extern uint32_t translate_addr(uint32_t addr);

extern "C" int pmem_read(uint32_t raddr) {
  uint32_t paddr = translate_addr(raddr);
  if (paddr == 0)
    return 0;
  if (paddr < MEM_BASE || paddr >= MEM_BASE + MEM_SIZE)
    return 0;

  uint8_t *host_ptr = guest_to_host(paddr & ~0x3u);
  return *(int *)host_ptr;
}

extern "C" void pmem_write(int waddr, int wdata, char wmask) {
  uint32_t paddr = translate_addr(waddr);
  if (paddr < MEM_BASE || paddr >= MEM_BASE + MEM_SIZE)
    return;

  uint8_t *p = guest_to_host(paddr & ~0x3u);
  for (int i = 0; i < 4; i++) {
    if ((wmask >> i) & 1) {
      p[i] = (uint8_t)((wdata >> (i * 8)) & 0xFF);
    }
  }
}