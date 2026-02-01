//#include <nvboard.h>
#include "svdpi.h"
#include <Vtop.h>
#include <cstdio>
#include <cstdlib>
#include <verilated.h>

static TOP_NAME dut;

//void nvboard_bind_all_pins(TOP_NAME *top);

static void single_cycle()
{
  dut.clk = 0;
  dut.eval();
  dut.clk = 1;
  dut.eval();
}

static void reset(int n)
{
  dut.rst = 1;
  while (n-- > 0)
    single_cycle();
  dut.rst = 0;
}

extern "C" void set_ebreak() {
  printf("\n---------- HIT EBREAK ----------\n");
  printf("NPC: Success! 程序执行完毕，正在退出仿真...\n");
  exit(0); // 直接退出整个仿真程序
}

uint32_t pmem_read(uint32_t addr) {
    if (addr == 0x80000000) return 0x00a00093; // addi x1, x0, 10
    if (addr == 0x80000004) return 0x00a08113; // addi x2, x1, 10
    return 0x00100073; // ebreak (当作停机)
}
int main()
{
  //nvboard_bind_all_pins(&dut);
  //nvboard_init();
  reset(10);

  while (1)
  {
    dut.inst = pmem_read(dut.pc);
    //nvboard_update();
    single_cycle();
    printf("PC = %08x | Inst = %08x | x1 = %d\n", dut.pc, dut.inst, dut.x1_status);
    
  }
}
