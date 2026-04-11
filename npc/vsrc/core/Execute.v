module Execute (
    input  [31:0] rdata1, rdata2,
    input  [31:0] imm32,
    input         alu_src,
    input  [2:0]  funct3,
    
    output [31:0] alu_res,
    output [31:0] jump_target,
    output [1:0]  alu_res_1_0,
    output [23:0] alu_res_25_2,
    output [3:0]  alu_res_31_28
);
    // ALU操作数选择
    wire [31:0] src2 = alu_src ? imm32 : rdata2;

    // 计算结果
    wire [31:0] alu_add   = rdata1 + src2;
    wire [31:0] alu_shift = rdata1 << imm32[4:0];
    assign alu_res = funct3[1] ? alu_shift : alu_add;

    // 计算跳转地址
    assign jump_target = alu_res & 32'hfffffffe;

    // 计算结果用于各类信号
    assign alu_res_1_0   = alu_res[1:0];
    assign alu_res_25_2  = alu_res[25:2];
    assign alu_res_31_28 = alu_res[31:28];
endmodule