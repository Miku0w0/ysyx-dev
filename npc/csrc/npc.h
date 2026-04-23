#ifndef __NPC_H__
#define __NPC_H__

#include "Vtop.h"
#include "verilated_vcd_c.h"
#include <cstdint>

// main.cpp
extern Vtop *dut;
extern VerilatedVcdC *tfp;
extern uint64_t sim_time;

// sim.cpp
void npc_init(int argc, char **argv);
void npc_exit();
void single_cycle();
void reset(int n);

// DPI-C 函数声明
extern "C" {
    void set_ebreak();
    int pmem_read(uint32_t raddr);
    void pmem_write(int waddr, int wdata, char wmask);
}

#endif