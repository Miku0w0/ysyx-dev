#include "../npc.h"

void npc_exit() {
  if (tfp) {
    tfp->flush();
    tfp->close();
  }
  dut->final();
  delete dut;
}