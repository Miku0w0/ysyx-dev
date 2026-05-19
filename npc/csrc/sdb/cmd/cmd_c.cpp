#include "../../npc.h"
#include "../sdb.h"

extern bool wp_triggered;

int cmd_c(char *args) {
    printf("Continuing execution...\n");
    while (!Verilated::gotFinish()) {
      wp_triggered = false;
      single_cycle();
      
      if (wp_triggered) {
        printf("\nStopped at:\n");
        printf("  PC = 0x%08x\n", get_pc());
        printf("  INST = 0x%08x\n", get_inst());
        return 0;
      }
    }
  return 0;
}