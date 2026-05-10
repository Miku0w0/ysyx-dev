module JumpTarget (
    input  [31:0] pc_i,
    input  [31:0] rdata1,
    input  [31:0] imm32,
    
    output [31:0] jalr_target,
    output [31:0] jal_target,
    output [31:0] branch_target
);
    assign jalr_target   = (rdata1 + imm32) & 32'hfffffffe; // 2字节对齐
    assign jal_target    = pc_i + imm32;
    assign branch_target = pc_i + imm32;
endmodule