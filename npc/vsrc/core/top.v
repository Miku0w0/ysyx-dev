import "DPI-C" function void set_ebreak();
import "DPI-C" function int  pmem_read(input int raddr);
import "DPI-C" function void pmem_write(input int waddr, input int wdata, input byte wmask);
module top (
    input clk,
    input reset,
    output [31:0] pc,   
    output [31:0] a0    
);
    /* IFU相关 */
    wire [31:0] inst, dnpc, snpc; 

    /* IDU相关 */
    wire [2:0]  funct3;
    wire [3:0]  alu_op;
    wire [4:0]  waddr, rs1, rs2;
    wire [31:0] imm32, u_imm;
    wire        alu_src, wen;
    wire        is_lui, is_auipc;
    wire        is_load, is_store;
    wire        is_sw, is_sb, is_sh; 
    wire        is_lw, is_lb, is_lbu, is_lh, is_lhu;
    wire        is_jalr, is_jal, is_branch, take_branch;

    /* EXU相关 */
    wire [31:0] rdata1, rdata2;
    wire [31:0] alu_res;
    wire [31:0] jalr_target, jal_target, branch_target;

    /* LSU相关 */
    wire        mem_we;
    wire [7:0]  mem_wmask;
    wire [31:0] mem_wdata_i;
    wire [31:0] mem_rdata_o;  

    /* WBU相关 */
    wire [31:0] wdata;
    
    /* 函数返回值 */
    assign a0 = u_rf.rf[10];

    REGFILE #(5, 32) u_rf (
        /* from top */
        .clk(clk),
        .reset(reset),
        /* from IDU */
        .wen(wen),       
        .waddr(waddr),
        .rs1(rs1),
        .rs2(rs2),    
        /* from WBU */
        .wdata(wdata),  

        /* to EXU */
        .rdata1(rdata1),
        .rdata2(rdata2)
    );
        
    IFU u_ifu (
        /* from top */
        .clk(clk),
        .reset(reset),
        /* from IDU */
        .is_jalr(is_jalr),
        .is_jal(is_jal),
        .is_branch(is_branch),
        /* from EXU */
        .jalr_target(jalr_target),
        .branch_target(branch_target),
        .jal_target(jal_target),
        .take_branch(take_branch),
        
        .dnpc(dnpc),
        /* to EXU */
        .pc(pc),
        /* to WBU */
        .snpc(snpc),
        /* to IDU */
        .inst(inst)
    );
        


    IDU u_idu (
        /* from IFU */
        .inst(inst),

        /* to rf */
        .waddr(waddr), 
        .wen(wen),
        .rs1(rs1), 
        .rs2(rs2), 

        .funct3(funct3),
        .alu_op(alu_op),
        .alu_src(alu_src),
        .u_imm(u_imm),
        .imm32(imm32),
        .is_load(is_load),
        .is_store(is_store),
        .is_lw(is_lw),
        .is_lb(is_lb),
        .is_lbu(is_lbu),
        .is_lh(is_lh),
        .is_lhu(is_lhu),
        .is_sw(is_sw),
        .is_sb(is_sb),
        .is_sh(is_sh),
        .is_lui(is_lui),
        .is_auipc(is_auipc),
        .is_jalr(is_jalr),
        .is_jal(is_jal),
        .is_branch(is_branch)        
    );

    EXU u_exu (
        /* from IFU */
        .pc_i(pc),
        /* from rf */
        .rdata1(rdata1),
        .rdata2(rdata2),
        /* from IDU */
        .imm32(imm32),
        .alu_op(alu_op),
        .is_auipc(is_auipc),
        .is_branch(is_branch),
        .alu_src(alu_src),
        .funct3(funct3),

        /* to IFU */
        .jalr_target(jalr_target),
        .jal_target(jal_target),
        .branch_target(branch_target),
        .take_branch(take_branch),
        /* to LSU && to WBU */
        .alu_res(alu_res)
    );
    
    LSU u_lsu (
        .clk(clk),
        /* from EXU */
        .alu_res(alu_res),
        /* from rf */
        .rdata2(rdata2),
        /* from IDU */
        .is_sw(is_sw),
        .is_sh(is_sh),
        .is_sb(is_sb),
        .is_lw(is_lw),  
        .is_lh(is_lh),   
        .is_lb(is_lb),
        .is_lhu(is_lhu), 
        .is_lbu(is_lbu),

        /* to WBU */
        .mem_we(mem_we),
        .mem_wmask(mem_wmask),
        .mem_rdata_o(mem_rdata_o),
        .mem_wdata_i(mem_wdata_i)
        );

    WBU u_wbu (
        /* from EXU */
        .alu_res(alu_res),
        /* from LSU */
        .mem_rdata_o(mem_rdata_o),   
        /* from IFU */
        .snpc(snpc),
        /* from IDU */
        .u_imm(u_imm),
        .is_load(is_load),
        .is_lui(is_lui),        
        .is_jalr(is_jalr),
        .is_jal(is_jal),

        /* to rf */
        .wdata_i(wdata)
    );

    // ========== 仿真日志 ==========
    // 计数器
    reg [31:0] inst_cnt; 
    always @(posedge clk) begin
    if (reset)
        inst_cnt <= 0;
    else
        inst_cnt <= inst_cnt + 1;
    end
    // 仿真
    always @(posedge clk) begin
        if (!reset) begin
            if ($test$plusargs("trace")) begin
            // ===== itrace =====
            $write("[%04d] PC:%08h  INST:%08h ", inst_cnt, pc, inst);
            // ===== 写回 =====
            if (wen && waddr != 0)
                $write("WB: x%02d<-%08h  ", waddr, wdata);
            else
                $write("WB: -------------  ");
            // ===== 访存 =====
            if (mem_we)
                $write("MEMW: [%08h]<-%08h (M:%b)", alu_res, mem_wdata_i, mem_wmask);
            else if (is_load)
                $write("MEMR: [%08h]->%08h", alu_res, mem_rdata_o);
            else
                $write("MEM: -----------");
            $write("\n");
            // ===== ebreak =====
            end
            if (inst == 32'h00100073) begin
                $display("[EBREAK] hit at PC=%08h", pc);
                set_ebreak();
                $finish;
            end
        end
    end
endmodule