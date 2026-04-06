module alu(
    input  [31:0] src1,
    input  [31:0] imm,
    output [31:0] alu_res
);
    assign alu_res = src1 + imm;
endmodule