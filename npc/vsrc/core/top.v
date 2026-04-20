import "DPI-C" function void set_ebreak();
import "DPI-C" function string disassemble(input int inst);
module top (
    input clk,
    input reset,

    output [31:0] pc,   
    output [31:0] a0    
);
    wire [31:0] inst;
    wire [31:0] dnpc, snpc, jalr_target, jal_target, branch_target;
    wire [23:0] rom_i;
    wire [31:0] rdata1, rdata2, wdata;
    wire [31:0] alu_res;
    wire [31:0] imm32, u_imm;
    wire [31:0] ram_o, ram_data_i;
    wire [31:0] ram_addr_i;
    
    wire [4:0]  waddr, rs1, rs2;
    wire [2:0]  funct3;
    wire [3:0]  alu_op;
    wire        wen, alu_src, is_jalr, is_jal, is_branch, take_branch;
    wire        is_load, is_lw, is_lb, is_lbu, is_lh, is_lhu, is_sw, is_sb, is_sh, is_lui, is_auipc;

    wire        ram_we;
    wire [3:0]  ram_wmask;
    wire [31:0] ram_o_raw;      
    wire [31:0] mem_rdata_out;  

    assign a0 = u_rf.rf[0];

    RegFile #(5, 32) u_rf (
        /* from top */
        .clk(clk),
        .reset(reset),
        /* from ID */
        .wen(wen),       
        .waddr(waddr),
        .rs1(rs1),
        .rs2(rs2),    
        /* from WB */
        .wdata(wdata),  

        /* to Ex */
        .rdata1(rdata1),
        .rdata2(rdata2)
    );
        
    InstructionFetch u_IF (
        /* from top */
        .clk(clk),
        .reset(reset),
        /* from ID */
        .is_jalr(is_jalr),
        .is_jal(is_jal),
        .is_branch(is_branch),
        /* from Ex */
        .jalr_target(jalr_target),
        .branch_target(branch_target),
        .jal_target(jal_target),
        .take_branch(take_branch),
        
        .dnpc(dnpc),
        /* to EX */
        .pc(pc),
        /* to WB */
        .snpc(snpc),
        /* to ROM */
        .rom_i(rom_i)
    );
        
    ROM u_rom (
        /* from IF */
        .rom_i(rom_i),
        
        /* to ID */
        .inst(inst)
    );

    InstructionDecode u_ID (
        /* from IF */
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

    Execute u_Ex (
        /* from IF */
        .pc_i(pc),
        /* from rf */
        .rdata1(rdata1),
        .rdata2(rdata2),
        /* from ID */
        .imm32(imm32),
        .alu_op(alu_op),
        .is_auipc(is_auipc),
        .is_jal(is_jal),
        .is_branch(is_branch),
        .alu_src(alu_src),
        .funct3(funct3),

        /* to IF */
        .jalr_target(jalr_target),
        .jal_target(jal_target),
        .branch_target(branch_target),
        .take_branch(take_branch),
        /* to Mem && to WB*/
        .alu_res(alu_res)
    );
    
    MemoryAccess u_Mem (
        /* from Ex */
        .alu_res(alu_res),
        .addr_offset(alu_res[1:0]),
        .addr_high(alu_res[31:28]),
        /* from RAM */
        .ram_o_raw(ram_o_raw),
        /* from rf */
        .rdata2(rdata2),
        /* from ID */
        .is_sw(is_sw),
        .is_sh(is_sh),
        .is_sb(is_sb),
        .is_lw(is_lw),  
        .is_lh(is_lh),   
        .is_lb(is_lb),
        .is_lhu(is_lhu), 
        .is_lbu(is_lbu),
        
        /* to RAM */
        .ram_addr_i(ram_addr_i),
        .ram_data_i(ram_data_i),
        .ram_we(ram_we),
        .ram_wmask(ram_wmask),
        /* to WB */
        .mem_rdata_out(mem_rdata_out)
        );
        
    RAM u_ram (
        /* from top */
        .clk(clk),
        /* from Mem */
        .ram_addr_i(ram_addr_i),
        .ram_data_i(ram_data_i),
        .ram_we(ram_we),       
        .ram_wmask(ram_wmask),    
        
        /* to Mem*/
        .ram_o_raw(ram_o_raw)
    );
        
    WriteBack u_WB (
        /* from Ex */
        .alu_res(alu_res),
        /* from Mem */
        .mem_rdata_out(mem_rdata_out),   
        /* from IF */
        .snpc(snpc),
        /* from ID */
        .u_imm(u_imm),
        .is_load(is_load),
        .is_lui(is_lui),        
        .is_jalr(is_jalr),
        .is_jal(is_jal),

        /* to rf */
        .wdata_i(wdata)
    );


    // 计数器
    reg [31:0] inst_cnt; 
    always @(posedge clk) begin
    if (reset)
        inst_cnt <= 0;
    else
        inst_cnt <= inst_cnt + 1;
    end
    // 反汇编结果
    string asm_str;
    // ========== 仿真日志 ==========
    always @(posedge clk) begin
    if (!reset) begin
        asm_str = disassemble(inst);
        // ===== itrace =====
        $write("[%04d] PC:%08h  INST:%08h  %-18s  ", inst_cnt, pc, inst, asm_str);
        // $display("branch=%b take=%b dnpc=%h", is_branch, take_branch, dnpc);
        // ===== 写回 =====
        if (wen && waddr != 0)
            $write("WB: x%02d=%08h  ", waddr, wdata);
        else
            $write("WB: ------------  ");
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