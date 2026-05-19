#include "../../npc.h"
#include "../sdb.h"

int cmd_c(char *args) {
  printf("Continuing execution...\n");
  while (!Verilated::gotFinish()) {
    single_cycle();
    // 检查监视点
    if (check_wp() > 0) {
      return 0;
    }
  }
  return 0;
}