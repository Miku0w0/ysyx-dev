import "DPI-C" function void set_ebreak();
module riscv32(
    input clk,
    input rst,
    input [31:0] inst,      
    output [31:0] pc,    
    output [31:0] x1_status
);
    wire [31:0] jump_target;
    wire [31:0] snpc = pc + 4;
    wire [31:0] dnpc;
    wire [4:0] rs1;
    wire [4:0] rd;
    wire [31:0] imm; 
    wire [31:0] src1;
    wire [31:0] alu_res;
    wire wen;
    wire is_jalr;
    wire is_ebreak;

    assign jump_target = (src1 + imm) & 32'hfffffffe; // 用于 jalr 的跳转地址
    assign dnpc = is_jalr ? jump_target : snpc;

    Reg #(32, 32'h80000000) u_pc (
        .clk(clk), 
        .rst(rst), 
        .din(dnpc), 
        .wen(1'b1),
        .dout(pc)
    );

    RegFile #(5, 32) u_rf (
        .clk(clk),
        .wdata(alu_res),  
        .waddr(rd),
        .wen(wen),       
        .raddr1(rs1),
        .rdata1(src1),    
        .x1_val(x1_status)
    );

    Decoder u_decoder (
        .inst(inst),
        .rs1(rs1), 
        .rd(rd), 
        .imm(imm),
        .is_jalr(is_jalr), 
        .is_ebreak(is_ebreak),
        .wen(wen)          
    );

    Write_Back u_write_back (

    )
    ALU u_alu (
        .src1(src1), 
        .imm(imm), 
        .alu_res(alu_res)
    );

    RV_Trap u_rv_trap (
            .clk(clk),
            .is_ebreak(is_ebreak),
            .pc(pc)
        );
endmodule