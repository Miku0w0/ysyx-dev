module InstructionFetch (
    /* from top */
    input clk,
    input reset,
    /* from ID */
    input is_jalr,
    /* from Ex */
    input [31:0] jump_target,
    
    output [31:0] pc,    // 当前pc
    output [31:0] snpc,  // 顺序执行的下一条pc
    output [23:0] rom_i  // 给 ROM 的地址改按字寻址
);
    wire [31:0] pc_val;  // 当前pc
    wire [31:0] dnpc;    // 下一拍写入pc的值

    Reg #(32, 32'h80000000) u_pc (
        .clk(clk),
        .rst(reset),
        .din(dnpc),     // 下一拍写入pc
        .wen(1'b1),

        .dout(pc_val)   // 当前pc
    );

    assign pc   = pc_val;
    assign snpc = pc_val + 32'd4;
    assign dnpc = is_jalr ? jump_target : snpc; // 下一条指令选择
    assign rom_i = pc_val[25:2]; // rom输入 改按字寻址

endmodule