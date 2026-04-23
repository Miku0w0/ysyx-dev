#include "npc.h"

Vtop *dut = nullptr;
VerilatedVcdC *tfp = nullptr;
uint64_t sim_time = 0;

void load_img(char *img_file);

int main(int argc, char **argv) {
  npc_init(argc, argv);
  if (argc > 1) load_img(argv[1]);
  reset(10);
  while (!Verilated::gotFinish()) single_cycle();
  npc_exit();
  return 0;
}