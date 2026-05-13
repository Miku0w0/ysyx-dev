#ifndef __NPC_H__
#define __NPC_H__

#include "Vtop.h"
#include "verilated_vcd_c.h"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// ========== 全局变量 ==========
extern Vtop *dut;
extern VerilatedVcdC *tfp;
extern uint64_t sim_time;
extern uint32_t inst_cnt;

// ========== 仿真控制 ==========
void npc_init(int argc, char **argv);
void npc_exit();
void single_cycle();
void reset(int n);

// ========== Trace ==========
void trace_init();
void trace_set_enable(bool enable);
void trace_record();
void trace_display();

// ========== 信号获取 ==========
uint32_t get_pc();
uint32_t get_inst();
uint32_t get_wdata();
uint32_t get_waddr();
bool get_wen();
uint32_t get_mem_addr();
uint32_t get_mem_wdata();
uint32_t get_mem_rdata();
bool get_mem_we();
bool get_is_load();
uint32_t get_a0();
uint32_t get_inst_cnt();

// ========== 内存加载 ==========
void load_img(char *img_file);

// ========== DPI-C函数声明 ==========
extern "C" {
    void set_ebreak();
    int pmem_read(uint32_t raddr);
    void pmem_write(int waddr, int wdata, char wmask);
}

#endif