#include "svdpi.h"
#include "verilated_vcd_c.h"
#include <Vriscv32.h>
#include <cstdio>
#include <cstdlib>
#include <verilated.h>

static Vriscv32 *dut;
static VerilatedVcdC *tfp;
static uint64_t sim_time = 0;

static void single_cycle() {
  dut->clk = 0;
  dut->eval();
  if (tfp)
    tfp->dump(sim_time++);

  dut->clk = 1;
  dut->eval();
  if (tfp)
    tfp->dump(sim_time++);
}

static void reset(int n) {
  dut->rst = 1;
  while (n-- > 0)
    single_cycle();
  dut->rst = 0;
}

extern "C" void set_ebreak() {
  printf("\n---------- HIT EBREAK ----------\n");
  printf("NPC: Success! 程序执行完毕，正在退出仿真...\n");
  if (tfp)
    tfp->close();
  exit(0); // 直接退出整个仿真程序
}

uint32_t pmem_read(uint32_t addr) {
switch (addr) {
    // 00000000 <_start>:
    case 0x80000000: return 0x01400513; // addi a0, zero, 20
    case 0x80000004: return 0x010000e7; // jalr ra, 16(zero) #10 <fun>
    case 0x80000008: return 0x00c000e7; // jalr ra, 12(zero) #c <halt>

    // 0000000c <halt>: (死循环)
    case 0x8000000c: return 0x00c00067; // jalr zero, 12(zero)

    // 00000010 <fun>:
    case 0x80000010: return 0x00a50513; // addi a0, a0, 10
    case 0x80000014: return 0x00008067; // jalr zero, 0(ra) #c <halt>

    default: return 0x00100073; // ebreak
  }
}
int main(int argc, char **argv) {
  Verilated::commandArgs(argc, argv);
  dut = new Vriscv32;
  Verilated::traceEverOn(true); // 开启追踪
  tfp = new VerilatedVcdC;
  dut->trace(tfp, 99); // 追踪深度
  tfp->open("build/obj_dir/Top.vcd"); 

  reset(10);
  while (1) {
    dut->inst = pmem_read(dut->pc);
    single_cycle();
    printf("PC = %08x | Inst = %08x | x1 = %d\n", dut->pc, dut->inst,
           dut->x1_status);
  }

  dut->final();
  tfp->close();
  return 0;
}
