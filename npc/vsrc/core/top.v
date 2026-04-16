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
    wire        is_load, is_lw, is_lbu, is_sw, is_sb, is_lui;
    wire        M3, M2, M7, M6, M5, M4;

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

        .snpc(snpc),
        .pc_o_25_2(rom_i)
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
        .is_lbu(is_lbu),
        .is_sw(is_sw),
        .is_sb(is_sb),
        .is_lui(is_lui),
        .is_jalr(is_jalr)        
    );

    Execute u_Ex (
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
        .alu_res_31_28(alu_res_31_28),
        .alu_res_25_2(alu_res_25_2),
        .alu_res_1_0(alu_res_1_0),
        .is_sw(is_sw),
        .is_sb(is_sb),
        .rdata2(rdata2),

        .ram_addr_i(ram_addr_i),
        .ram_data_i(ram_data_i),
        .M3(M3),
        .M2(M2),
        .M7(M7),
        .M6(M6),
        .M5(M5),
        .M4(M4)
    );

    WriteBack u_WB (
        .alu_res_1_0(alu_res_1_0),
        .alu_res(alu_res),
        .ram_o(ram_o),
        .snpc(snpc),
        .u_imm(u_imm),
        .is_load(is_load),
        .is_lw(is_lw),
        .is_lbu(is_lbu),
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
        .M3(M3),
        .M2(M2),
        .M7(M7),
        .M6(M6),
        .M5(M5),
        .M4(M4),

        .ram_o(ram_o)
    );

    always @(posedge clk) begin
        if (!reset) begin
            $write("PC: %h | Inst: %h ", (u_IF.pc_o_25_2 << 2), inst);
            if (wen && rd != 0) begin
                $write("| WRITE x%02d = %h", rd, wdata_i);
            end
            $write("\n"); 
        end

        if (is_load || is_sw) begin
            $display("  [DEBUG] RAM_ADDR: %h | RAM_DATA_O: %h | ALU_RES: %h", 
                     ram_addr_i, ram_o, alu_res);
        end
    end

    always @(posedge clk) begin
        if (!reset && inst == 32'h00100073) begin
            $display("--- SUCCESS: ebreak detected, terminating simulation ---");
            set_ebreak(); 
            $finish;     
        end
    end
endmodule