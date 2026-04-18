import "DPI-C" function void set_ebreak();
module top (
    input clk,
    input reset
);
    wire [31:0] inst;
    wire [31:0] pc, snpc, jump_target;
    wire [23:0] rom_i;
    wire [31:0] rdata1, rdata2, wdata_i;
    wire [31:0] alu_res;
    wire [3:0]  alu_res_31_28;
    wire [23:0] alu_res_25_2;
    wire [1:0]  alu_res_1_0;
    wire [31:0] imm32, u_imm;
    wire [31:0] ram_o, ram_data_i;
    wire [23:0] ram_addr_i;
    
    wire [4:0]  rd, rs1, rs2;
    wire [2:0]  funct3;
    wire [3:0]  alu_op;
    wire        wen, alu_src, is_jalr;
    wire        is_load, is_lw, is_lb, is_lbu, is_lh, is_lhu, is_sw, is_sb, is_sh, is_lui;

    wire        ram_we;
    wire [3:0]  ram_wmask;
    wire [31:0] ram_o_raw;      // 对应 MemoryAccess 的输入
    wire [31:0] mem_rdata_out;  // 对应 MemoryAccess 的输出


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

        .addr_word(alu_res[23:0]),
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

    RAM u_ram (
        .clk(clk),
        .ram_addr_i(ram_addr_i),
        .ram_data_i(ram_data_i),
        .ram_we(ram_we),       // 连向 MemoryAccess 的 ram_we
        .ram_wmask(ram_wmask),    // 连向 MemoryAccess 的 ram_wmask

        .mem_rdata_raw(ram_o_raw)     // 注意端口名要和 RAM 模块内定义的一致
    );

    always @(posedge clk) begin
        if (!reset) begin
            // PC 与 指令 
            $write("PC: %h  Inst: %h  ", pc, inst);

            // 寄存器写回对齐
            if (wen && rd != 0)
                $write("W: [x%02d: %h]  ", rd, wdata_i);
            else
                $write("W: [-------------]  "); // 保持列宽对齐

            // 访存操作对齐
            if (ram_we)
                $write("MEM_W: [%h] <- %h (M:%b)", alu_res, ram_data_i, ram_wmask);
            else if (is_load)
                $write("MEM_R: [%h] -> %h", alu_res, mem_rdata_out);
            else
                $write("                              "); // 留白，防止换行符错位

            $write("\n");
            // 内存读取
            if (is_load) begin
                $display("[RAM_READ ] Byte_Addr: %h | Word_Idx: %h | Raw_Data: %h | Offset: %d", 
                         alu_res[23:0], alu_res[23:2], ram_o_raw, alu_res[1:0]);
            end
            // 内存写入
            if (ram_we) begin
                $display("[RAM_WRITE] Byte_Addr: %h | Word_Idx: %h | Data: %h | Mask: %b", 
                         ram_addr_i, ram_addr_i[23:2], ram_data_i, ram_wmask);
            end
            // 停机逻辑
            if (inst == 32'h00100073) begin
                $display("[EBREAK] Program Hit EBREAK. Simulation Finished.");
                set_ebreak();
                $finish;
            end
        end
    end
endmodule