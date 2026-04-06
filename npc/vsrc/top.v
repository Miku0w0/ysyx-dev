import "DPI-C" function void set_ebreak();
module top(
    input clk,
    input rst,
    input [31:0] inst,      
    output [31:0] pc,    
    output [31:0] x1_status
);

    wire [31:0] snpc = pc + 4;
    wire [4:0] rs1;
    wire [4:0] rd;
    wire [31:0] imm; 
    wire [31:0] src1;
    wire [31:0] alu_res;
    wire is_ebreak;

    decoder u_decoder (
        .inst(inst),
        .rs1(rs1), 
        .rd(rd), 
        .imm(imm),
        .is_ebreak(is_ebreak)
    );

    alu u_alu (
        .src1(src1), 
        .imm(imm), 
        .alu_res(alu_res)
    );

    reg #(32, 32'h80000000) u_pc (
        .clk(clk), 
        .rst(rst), 
        .din(snpc), 
        .wen(1'b1),
        .dout(pc)
    );

    regfile #(5, 32) u_rf (
        .clk(clk),
        .wdata(alu_res),  
        .waddr(rd),
        .wen(1'b1),       // 暂时假设所有指令都写回（如addi）
        .raddr1(rs1),
        .rdata1(src1),    // 读出的数据连到 src1 线路上
        .x1_val(x1_status)
    );

    rv_trap u_rv_trap (
            .clk(clk),
            .is_ebreak(is_ebreak),
            .pc(pc)
        );
endmodule