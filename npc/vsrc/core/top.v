import "DPI-C" function void set_ebreak();
import "DPI-C" function int  pmem_read(input int raddr);
import "DPI-C" function void pmem_write(input int waddr, input int wdata, input byte wmask);
module top (
    input clk,
    input reset,

    output [31:0] pc,   
    output [31:0] a0    
);
    wire [31:0] inst;
    wire [31:0] dnpc, snpc, jalr_target, jal_target, branch_target;
    wire [31:0] rdata1, rdata2, wdata;
    wire [31:0] alu_res;
    wire [31:0] imm32, u_imm;
    //wire [31:0] rom_i;
    //wire [31:0] ram_o, ram_data_i;
    wire [31:0] ram_data_i;
    //wire [31:0] ram_addr_i;
    
    wire [4:0]  waddr, rs1, rs2;
    wire [2:0]  funct3;
    wire [3:0]  alu_op;
    wire        wen, alu_src, is_jalr, is_jal, is_branch, take_branch;
    wire        is_load, is_lw, is_lb, is_lbu, is_lh, is_lhu, is_sw, is_sb, is_sh, is_lui, is_auipc;

    wire        ram_we;
    wire [7:0]  ram_wmask;
    //wire [31:0] ram_data_o;      
    wire [31:0] mem_rdata_out;  

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
        /* to ROM */
        //.rom_i(rom_i)
    );
        
    //ROM u_rom (
        /* from IFU */
        //.rom_i(rom_i),
        
        /* to IDU */
        //.inst(inst)
    //);

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
        .is_jal(is_jal),
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
        /* from RAM */
        //.ram_data_o(ram_data_o),
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
        
        /* to RAM */
        //.ram_addr_i(ram_addr_i),
        .ram_data_i(ram_data_i),
        .ram_we(ram_we),
        .ram_wmask(ram_wmask),
        /* to WBU */
        .mem_rdata_out(mem_rdata_out)
        );
        
    //RAM u_ram (
        /* from top */
        //.clk(clk),
        /* from LSU */
        //.ram_addr_i(ram_addr_i),
        //.ram_data_i(ram_data_i),
        //.ram_we(ram_we),       
        //.ram_wmask(ram_wmask),    
        
        /* to LSU*/
        //.ram_data_o(ram_data_o)
    //);
        
    WBU u_wbu (
        /* from EXU */
        .alu_res(alu_res),
        /* from LSU */
        .mem_rdata_out(mem_rdata_out),   
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
            // ===== itrace =====
            $write("[%04d] PC:%08h  INST:%08h ", inst_cnt, pc, inst);
            // ===== 写回 =====
            if (wen && waddr != 0)
                $write("WB: x%02d<-%08h  ", waddr, wdata);
            else
                $write("WB: -------------  ");
            // ===== 访存 =====
            if (ram_we)
                $write("MEMW: [%08h]<-%08h (M:%b)", alu_res, ram_data_i, ram_wmask);
            else if (is_load)
                $write("MEMR: [%08h]->%08h", alu_res, mem_rdata_out);
            else
                $write("MEM: -----------");
            $write("\n");
            // ===== ebreak =====
            if (inst == 32'h00100073) begin
                $display("[EBREAK] hit at PC=%08h", pc);
                set_ebreak();
                $finish;
            end
        end
    end
endmodule