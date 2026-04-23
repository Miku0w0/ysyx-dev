#include "npc.h"
#include <cstdio>
#include <cstdlib>

extern "C" void set_ebreak() {
  printf("\033[1;32m[NPC] HIT EBREAK AT PC = 0x%08x\033[0m\n", dut->pc);

  if (dut->a0 == 0) {
    printf("\033[1;32m[NPC] HIT GOOD TRAP!\033[0m\n");
  } else {
    printf("\033[1;31m[NPC] HIT BAD TRAP! (code: 0x%08x)\033[0m\n", dut->a0);
  }

  if (tfp)  tfp->close();

  exit(0);
}