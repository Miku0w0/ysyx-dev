module InstructionDecode (
    /* from IF */
    input  [31:0] inst,
    /* Field Extraction */
    output [4:0]  waddr,    // 寄存器rd索引
    output [4:0]  rs1,      // rdata1索引
    output [4:0]  rs2,      // rdata2索引
    output [2:0]  funct3,   // 子操作码
    output [31:0] u_imm,    // 立即数用于lui auipc
    output [31:0] imm32,    // 最终选出的立即数
    /* Control Signals */
    output        wen,      // 寄存器写使能
    output [3:0]  alu_op,   // alu运算类型
    output        alu_src,  // alu第二个操作数选择（0寄存器 1立即数） 
    /* Instruction Flags */
    output        is_load,  
    output        is_lw,
    output        is_lb,
    output        is_lbu,
    output        is_lh,   
    output        is_lhu,   
    output        is_sw,
    output        is_sb,
    output        is_sh,  
    output        is_lui,
    output        is_jalr
);
    wire [6:0] opcode = inst[6:0];  // 指令类型
    assign waddr  = inst[11:7];     // 写回寄存器rd索引
    assign funct3 = inst[14:12];    // 子操作码
    assign rs1    = inst[19:15];    // 源寄存器1
    assign rs2    = inst[24:20];    // 源寄存器2

    wire   op_reg   = (opcode == 7'h33); // 寄存器-寄存器R
    wire   op_imm   = (opcode == 7'h13); // 寄存器-立即数I
    assign is_lui   = (opcode == 7'h37); // 高位加载LUI
    wire   is_auipc = (opcode == 7'h17); // 高位地址AUIPC
    assign is_jalr  = (opcode == 7'h67); // 间接跳转JALR
    assign is_load  = (opcode == 7'h03); // 加载Load
    wire   is_store = (opcode == 7'h23); // 存储Store

    assign is_lw  = is_load  && (funct3 == 3'b010);
    assign is_lh  = is_load  && (funct3 == 3'b001); 
    assign is_lb  = is_load  && (funct3 == 3'b000);
    assign is_lhu = is_load  && (funct3 == 3'b101);
    assign is_lbu = is_load  && (funct3 == 3'b100); 

    assign is_sw  = is_store && (funct3 == 3'b010); 
    assign is_sh  = is_store && (funct3 == 3'b001);
    assign is_sb  = is_store && (funct3 == 3'b000); 
 
    assign wen = op_reg | op_imm | is_load | is_lui | is_auipc| is_jalr;   // 写回寄存器的指令
    assign alu_src = ~op_reg;                                              // 第二操作数来源
    assign alu_op = (is_load | is_store | is_auipc) ? 4'b0000 :            // 统一加法
                                           (op_reg) ? {inst[30], funct3} : // R区分add sub 
                                           {1'b0, funct3};                 // I只有add

    wire [31:0] i_imm = {{20{inst[31]}}, inst[31:20]};                     // I-type
    wire [31:0] s_imm = {{20{inst[31]}}, inst[31:25], inst[11:7]};         // S-type

    assign u_imm = {inst[31:12], 12'b0};                                   // U-type
    assign imm32 = is_store ? s_imm :                                      // 最终选出的立即数
        (is_lui | is_auipc) ? u_imm : 
        i_imm;
    
endmodule