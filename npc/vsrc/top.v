import "DPI-C" function void set_ebreak();

module top(
    input clk,
    input rst,
    input [31:0] inst,      
    output [31:0] pc,    
    output [31:0] x1_status
);
    /* PC */
    wire [31:0] snpc = pc + 4;
    /* 译码 */
    wire [4:0] rs1;
    wire [4:0] rd;
    wire [31:0] imm; 
    /* 寄存器堆 */
    wire [31:0] src1;
    wire [31:0] alu_res;
    /* ebreak */
    wire is_ebreak;

    /* 实例化 */
    Reg #(32, 32'h80000000) pc_reg_inst (
        .clk(clk), .rst(rst), .din(snpc), .dout(pc), .wen(1'b1)
    );

    Decoder decoder_inst (
        .inst(inst),
        .rs1(rs1), .rd(rd), .imm(imm),
        .is_ebreak(is_ebreak)
    );

    Alu alu_inst (
        .src1(src1), .imm(imm), .alu_res(alu_res)
    );

    RegisterFile #(5, 32) rf_inst (
        .clk(clk),
        .wdata(alu_res),  // ALU的结果绕回来，连到写数据端口
        .waddr(rd),
        .wen(1'b1),       // 暂时假设所有指令都写回（如addi）
        .raddr1(rs1),
        .rdata1(src1),    // 读出的数据连到 src1 线路上
        .x1_val(x1_status)
    );

    RV_TRAP trap_inst (
            .clk(clk),
            .is_ebreak(is_ebreak),
            .pc(pc)
        );
endmodule