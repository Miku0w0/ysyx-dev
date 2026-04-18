#include <stdint.h>
#include <stdio.h>

extern "C" const char *disassemble(int inst) {
  static char buf[64];

  uint32_t opcode = inst & 0x7f;
  uint32_t rd = (inst >> 7) & 0x1f;
  uint32_t rs1 = (inst >> 15) & 0x1f;
  uint32_t rs2 = (inst >> 20) & 0x1f;
  uint32_t funct3 = (inst >> 12) & 0x7;

  switch (opcode) {
  case 0x13:
    sprintf(buf, "addi x%d,x%d,imm", rd, rs1);
    break;

  case 0x33:
    sprintf(buf, "add x%d,x%d,x%d", rd, rs1, rs2);
    break;

  case 0x03:
    sprintf(buf, "load x%d,(x%d)", rd, rs1);
    break;

  case 0x23:
    sprintf(buf, "store x%d,(x%d)", rs2, rs1);
    break;

  case 0x37:
    sprintf(buf, "lui x%d", rd);
    break;

  case 0x6f:
    sprintf(buf, "jal x%d", rd);
    break;

  case 0x73:
    sprintf(buf, "ebreak");
    break;

  default:
    sprintf(buf, "unknown %08x", inst);
    break;
  }

  return buf;
}