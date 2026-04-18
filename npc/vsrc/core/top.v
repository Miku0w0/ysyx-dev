import "DPI-C" function void set_ebreak();
import "DPI-C" function string disassemble(input int inst);
module top (
    input clk,
    input reset,

    output [31:0] pc,   
    output [31:0] a0    
);
    wire [31:0] inst;
    wire [31:0] snpc, jump_target;
    wire [23:0] rom_i;
    wire [31:0] rdata1, rdata2, wdata_i;
    wire [31:0] alu_res;
    wire [3:0]  alu_res_31_28;
    wire [23:0] alu_res_25_2;
    wire [1:0]  alu_res_1_0;
    wire [31:0] imm32, u_imm;
    wire [31:0] ram_o, ram_data_i;
    wire [31:0] ram_addr_i;
    
    wire [4:0]  rd, rs1, rs2;
    wire [2:0]  funct3;
    wire [3:0]  alu_op;
    wire        wen, alu_src, is_jalr;
    wire        is_load, is_lw, is_lb, is_lbu, is_lh, is_lhu, is_sw, is_sb, is_sh, is_lui;

    wire        ram_we;
    wire [3:0]  ram_wmask;
    wire [31:0] ram_o_raw;      // 对应 MemoryAccess 的输入
    wire [31:0] mem_rdata_out;  // 对应 MemoryAccess 的输出

    assign a0 = u_rf.rf[0];

    RegFile #(5, 32) u_rf (
        .clk(clk),
        .reset(reset),
        .wen(wen),       
        .wdata(wdata_i),  
        .waddr(rd),
        .rs1(rs1),
        .rs2(rs2),    

        .rdata1(rdata1),
        .rdata2(rdata2)
    );

    InstructionFetch u_IF (
        .clk(clk),
        .reset(reset),
        .is_jalr(is_jalr),
        .jump_target(jump_target),
        
        .pc(pc),
        .snpc(snpc),
        .rom_i(rom_i)
    );

    InstructionDecode u_ID (
        .inst(inst),

        .rd(rd), 
        .rs1(rs1), 
        .rs2(rs2), 
        .funct3(funct3),
        .alu_op(alu_op),
        .alu_src(alu_src),
        .wen(wen),
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
        .is_jalr(is_jalr)        
    );

    Execute u_Ex (
        .pc_i(pc),
        .rdata1(rdata1),
        .rdata2(rdata2),
        .imm32(imm32),
        .alu_op(alu_op),
        .alu_src(alu_src),
        .funct3(funct3),

        .jump_target(jump_target),
        .alu_res(alu_res),
        .alu_res_31_28(alu_res_31_28),
        .alu_res_25_2(alu_res_25_2),
        .alu_res_1_0(alu_res_1_0)
    );

    MemoryAccess u_Mem (
        .addr_word(alu_res),
        .addr_offset(alu_res[1:0]),
        .addr_high(alu_res_31_28),
        .is_sw(is_sw),
        .is_sh(is_sh),
        .is_sb(is_sb),
        .is_lw(is_lw),  
        .is_lh(is_lh),   
        .is_lb(is_lb),
        .is_lhu(is_lhu), 
        .is_lbu(is_lbu),
        .rdata2(rdata2),

        .ram_addr_i(ram_addr_i),
        .ram_data_i(ram_data_i),
        .ram_we(ram_we),
        .ram_wmask(ram_wmask),
        
        .ram_o_raw(ram_o_raw),      // RAM 直接出来的 32 位
        .mem_rdata_out(mem_rdata_out)
        );

    RAM u_ram (
        .clk(clk),
        .ram_addr_i(ram_addr_i),
        .ram_data_i(ram_data_i),
        .ram_we(ram_we),       // 连向 MemoryAccess 的 ram_we
        .ram_wmask(ram_wmask),    // 连向 MemoryAccess 的 ram_wmask

        .mem_rdata_raw(ram_o_raw)     // 注意端口名要和 RAM 模块内定义的一致
    );
        
    WriteBack u_WB (
        .alu_res(alu_res),
        .mem_rdata_out(mem_rdata_out),   
        .snpc(snpc),
        .u_imm(u_imm),
        .is_load(is_load),
        .is_lui(is_lui),        
        .is_jalr(is_jalr),

        .wdata_i(wdata_i)
    );

    ROM u_rom (
        .rom_i(rom_i),

        .inst(inst)
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
    // 测试验证逻辑
    always @(posedge clk) begin
    if (!reset) begin
        asm_str = disassemble(inst);
        // =====主 trace =====
        $write("[%04d] PC:%08h  INST:%08h  %-18s  ", inst_cnt, pc, inst, asm_str);
        // ===== 写回 =====
        if (wen && rd != 0)
            $write("WB: x%02d=%08h  ", rd, wdata_i);
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