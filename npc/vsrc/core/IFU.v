module IFU (
    /* from top */
    input clk,
    input reset,
    /* from IDU */
    input is_jalr,
    input is_jal,
    input is_branch,
    /* from EXU */
    input take_branch,
    input [31:0] branch_target,
    input [31:0] jalr_target,
    input [31:0] jal_target,
    
    output [31:0] dnpc,  // 最终下一拍跳转位置
    /* to EXU */
    output [31:0] pc,    // 当前pc
    /* to WBU */
    output [31:0] snpc,  // 顺序执行的下一条pc
    /* to ROM */
    output [23:0] rom_i  // 给 ROM 的地址改按字寻址
);
    wire [31:0] pc_val;  // 当前pc

    REG #(32, 32'h80000000) u_pc (
        .clk(clk),
        .rst(reset),
        .din(dnpc),     // 下一拍写入pc
        .wen(1'b1),

        .dout(pc_val)   // 当前pc
    );

    assign pc   = pc_val;
    assign snpc = pc_val + 32'd4;
    /* 下一条指令选择 */
    assign dnpc = is_jalr ? jalr_target    :
                  is_jal  ? jal_target     :
 is_branch && take_branch ? branch_target  : 
                            snpc;
    assign rom_i = pc_val[25:2]; // rom输入 改按字寻址

endmodule