#include "../npc.h"

// ========== 全局变量定义 ==========
Vtop *dut = nullptr;
VerilatedVcdC *tfp = nullptr;
uint64_t sim_time = 0;
uint32_t inst_cnt = 0;
bool sdb_enabled = false;