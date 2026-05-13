#include "npc.h"
#include <cstdio>
#include <cstdlib>

extern "C" void set_ebreak() {
  printf("\033[1;38;5;28m[NPC] HIT EBREAK AT PC = 0x%08x\033[0m\n", dut->pc);

  if (dut->a0 == 0) {
    printf("\033[1;38;5;28m[NPC] HIT GOOD TRAP!\033[0m\n");
    exit(0); // BAD TRAP -> 失败退出
  } else {
    printf("\033[1;38;5;196m[NPC] HIT BAD TRAP! (code: 0x%08x)\033[0m\n",
           dut->a0);
    exit(1); // BAD TRAP -> 失败退出
  }

  if (tfp)  tfp->close();


}