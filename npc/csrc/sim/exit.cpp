#include "../npc.h"

void npc_exit() {
  trace_display();
  if (tfp) {
    tfp->flush();
    tfp->close();
  }
  dut->final();
  delete dut;
}