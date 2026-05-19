#include "../../npc.h"
#include "../sdb.h"

int cmd_si(char *args) {
  int n = 1;
  if (args != NULL)
    sscanf(args, "%d", &n);

  for (int i = 0; i < n; i++) {
    single_cycle();
    printf("[%04d] PC:0x%08x  INST:0x%08x\n", get_inst_cnt(), get_pc(),
           get_inst());
  }
  return 0;
}