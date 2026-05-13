import "DPI-C" function void set_ebreak();
import "DPI-C" function int  pmem_read(input int raddr);
import "DPI-C" function void pmem_write(input int waddr, input int wdata, input byte wmask);
module top (
    input clk,
    input reset,
    output [31:0] pc, a0,
    output [31:0] inst, wdata, alu_res, mem_wdata_i, mem_rdata_o,
    output [31:0] dnpc, snpc,
    output [31:0] imm32, u_imm,
    output [31:0] rdata1, rdata2,
    output [31:0] jalr_target, jal_target, branch_target,
    output [4:0]  waddr, rs1, rs2,
    output [2:0]  funct3,
    output [3:0]  alu_op,
    output [7:0]  mem_wmask,
    output        wen, mem_we, is_load, alu_src,
    output        is_lui, is_auipc, 
    output        is_jalr, is_jal, is_branch, take_branch,
    output        is_store,
    output        is_lw, is_lb, is_lbu, is_lh, is_lhu,
    output        is_sw, is_sb, is_sh
);
    /* 函数返回值 */
    assign a0 = u_rf.rf[10];

    IFU u_ifu (
        .clk(clk), .reset(reset),
        .is_jalr(is_jalr), .is_jal(is_jal), .is_branch(is_branch),
        .jalr_target(jalr_target), .branch_target(branch_target),
        .jal_target(jal_target), .take_branch(take_branch),
        
        .dnpc(dnpc), .pc(pc), .snpc(snpc), .inst(inst)
    );
        
    IDU u_idu (
        .inst(inst),
        
        .waddr(waddr), .wen(wen), .rs1(rs1), .rs2(rs2),
        .funct3(funct3), .alu_op(alu_op), .alu_src(alu_src),
        .u_imm(u_imm), .imm32(imm32),
        .is_load(is_load), .is_store(is_store),
        .is_lw(is_lw), .is_lb(is_lb), .is_lbu(is_lbu),
        .is_lh(is_lh), .is_lhu(is_lhu),
        .is_sw(is_sw), .is_sb(is_sb), .is_sh(is_sh),
        .is_lui(is_lui), .is_auipc(is_auipc),
        .is_jalr(is_jalr), .is_jal(is_jal), .is_branch(is_branch)
    );

    EXU u_exu (
        .pc_i(pc),
        .rdata1(rdata1), .rdata2(rdata2), .imm32(imm32),
        .alu_op(alu_op), .alu_src(alu_src), .funct3(funct3),
        .is_auipc(is_auipc), .is_branch(is_branch),
        
        .jalr_target(jalr_target), .jal_target(jal_target),
        .branch_target(branch_target), .take_branch(take_branch),
        .alu_res(alu_res)
    );
    
    LSU u_lsu (
        .clk(clk),
        .alu_res(alu_res), .rdata2(rdata2),
        .is_sw(is_sw), .is_sh(is_sh), .is_sb(is_sb),
        .is_lw(is_lw), .is_lh(is_lh), .is_lb(is_lb),
        .is_lhu(is_lhu), .is_lbu(is_lbu),
        
        .mem_we(mem_we), .mem_wmask(mem_wmask),
        .mem_wdata_i(mem_wdata_i), .mem_rdata_o(mem_rdata_o)
    );

    WBU u_wbu (
        .alu_res(alu_res), .mem_rdata_o(mem_rdata_o),
        .snpc(snpc), .u_imm(u_imm),
        .is_lui(is_lui), .is_load(is_load),
        .is_jal(is_jal), .is_jalr(is_jalr),
        
        .wdata_i(wdata)
    );

    REGFILE #(5, 32) u_rf (
        .clk(clk), .reset(reset),
        .wen(wen), .waddr(waddr), .wdata(wdata),
        .rs1(rs1), .rs2(rs2),

        .rdata1(rdata1), .rdata2(rdata2)
    );

    always @(posedge clk) begin
        if (!reset && inst == 32'h00100073) begin
            $display("[EBREAK] hit at PC=%08h, a0=%08h", pc, a0);
            set_ebreak();
            $finish;
        end
    end

endmodule