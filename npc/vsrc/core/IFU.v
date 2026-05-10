module IFU (
    /* from top */
    input clk, reset,
    /* from IDU */
    input is_jalr, is_jal, is_branch,
    /* from EXU */
    input take_branch,
    input [31:0] branch_target, jalr_target, jal_target,
    
    output [31:0] pc, dnpc, snpc, inst
);
    /* 实例化pc */
    wire [31:0] pc_val;
    REG #(32, 32'h80000000) u_pc (
        .clk(clk),
        .rst(reset),
        .din(dnpc),     // 下一拍写入pc
        .wen(1'b1),

        .dout(pc_val)   // 当前pc
    );
    /* pc相关输出 */
    assign pc   = pc_val;
    assign snpc = pc_val + 32'd4;
    
    /* 下一条指令选择 */
    reg [31:0] dnpc_out;
    always @(*) begin
        if (is_jalr)                        dnpc_out = jalr_target;
        else if (is_jal)                    dnpc_out = jal_target;
        else if (is_branch && take_branch)  dnpc_out = branch_target;
        else                                dnpc_out = snpc;
    end
    assign dnpc = dnpc_out;
    /* 读取指令 */
    assign inst = pmem_read({pc_val[31:2], 2'b0});             

endmodule