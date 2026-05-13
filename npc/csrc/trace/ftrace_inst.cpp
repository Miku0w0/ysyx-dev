#include "trace.h"

// call jal
bool is_call_inst(uint32_t inst, uint32_t pc, uint32_t &target) {
  uint32_t opcode = inst & 0x7f;
  if (opcode == 0x6f) { // jal
    uint32_t rd = (inst >> 7) & 0x1f;
    if (rd == 1 || rd == 5) { // ra 或 t0
      int32_t imm = 0;
      imm |= (inst >> 21) & 0x3ff;        // imm[10:1]
      imm |= ((inst >> 20) & 0x1) << 11;  // imm[11]
      imm |= ((inst >> 12) & 0xff) << 12; // imm[19:12]
      imm |= ((inst >> 31) & 0x1) << 20;  // imm[20]
      imm = (imm << 11) >> 11;            // 符号扩展
      target = pc + imm;
      return true;
    }
  }
  return false;
}

// ret jalr
bool is_ret_inst(uint32_t inst) {
  uint32_t opcode = inst & 0x7f;
  if (opcode == 0x67) { // jalr
    uint32_t rd = (inst >> 7) & 0x1f;
    uint32_t rs1 = (inst >> 15) & 0x1f;
    int32_t imm = (int32_t)inst >> 20;
    return (rd == 0 && rs1 == 1 && imm == 0);
  }
  return false;
}