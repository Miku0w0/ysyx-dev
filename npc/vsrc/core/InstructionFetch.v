module InstructionFetch (
    input clk,
    input reset,
    input is_jalr,
    input [31:0] jump_target,
    
    output [31:0] pc,
    output [31:0] snpc,      // 传给下一阶段或写回
    output [23:0] rom_i  // 传给 ROM 的地址
);
    wire [31:0] pc_val;
    wire [31:0] dnpc;

    Reg #(32, 32'h80000000) u_pc (
        .clk(clk),
        .rst(reset),
        .din(dnpc),
        .wen(1'b1),
        .dout(pc_val)
    );

    assign pc   = pc_val;
    assign snpc = pc_val + 32'd4;
    assign dnpc = is_jalr ? jump_target : snpc; // 下一条指令选择
    assign rom_i = pc_val[25:2]; // rom输入

endmodule