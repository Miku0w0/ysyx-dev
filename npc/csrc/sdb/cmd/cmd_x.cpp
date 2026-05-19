#include "../../npc.h"
#include "../sdb.h"

int cmd_x(char *args) {
  if (args == NULL) {
    printf("Usage: x N EXPR\n");
    return 0;
  }

  int n;
  uint32_t addr;
  if (sscanf(args, "%d %x", &n, &addr) != 2) {
    printf("Usage: x N EXPR\n");
    return 0;
  }

  for (int i = 0; i < n; i++) {
    printf("0x%08x: 0x%08x\n", addr + i * 4, pmem_read(addr + i * 4));
  }
  return 0;
}