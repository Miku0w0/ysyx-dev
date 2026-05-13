#include "../npc.h"
#include <cstdint>

static uint8_t mem[MEM_SIZE];

uint8_t *guest_to_host(uint32_t paddr) { return mem + (paddr - MEM_BASE); }

uint32_t translate_addr(uint32_t addr) {
  if (addr != 0 && addr < MEM_BASE) {
    return addr + MEM_BASE;
  }
  return addr;
}