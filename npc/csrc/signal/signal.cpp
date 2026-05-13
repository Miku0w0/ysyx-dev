#include "../npc.h"
// ========== 获取信号值 ==========
uint32_t get_pc()        {return dut->pc; }
uint32_t get_inst()      {return dut->inst; }
uint32_t get_wdata()     {return dut->wdata; }
uint32_t get_waddr()     {return dut->waddr; }
bool     get_wen()       {return dut->wen; }
uint32_t get_mem_addr()  {return dut->alu_res; }
uint32_t get_mem_wdata() {return dut->mem_wdata_i; }
uint32_t get_mem_rdata() {return dut->mem_rdata_o; }
bool     get_mem_we()    {return dut->mem_we; }
bool     get_is_load()   {return dut->is_load; }
uint32_t get_a0()        {return dut->a0; }
uint32_t get_inst_cnt()  {return inst_cnt; }