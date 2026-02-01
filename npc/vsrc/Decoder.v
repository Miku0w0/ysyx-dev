module Decoder(
    input  [31:0] inst,
    output [4:0]  rs1,
    output [4:0]  rd,
    output [31:0] imm,
    output        is_ebreak
);
    assign rs1 = inst[19:15];
    assign rd  = inst[11:7];
    // I-type 立即数符号扩展
    assign imm = {{20{inst[31]}}, inst[31:20]};
    // 译码判断是否是 ebreak
    assign is_ebreak = (inst == 32'h00100073);
endmodule