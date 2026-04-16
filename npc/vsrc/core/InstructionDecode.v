module InstructionDecode (
    input  [31:0] inst,

    output [4:0]  rd,
    output [4:0]  rs1,
    output [4:0]  rs2,

    output [2:0]  funct3,
    output [3:0]  alu_op,
    output        alu_src,
    output        wen,
    output [31:0] u_imm,
    output [31:0] imm32,

    output        is_load,
    output        is_lw,
    output        is_lbu,
    output        is_sw,
    output        is_sb,
    output        is_lui,
    output        is_jalr
);
    wire [6:0] opcode = inst[6:0];
    assign rd     = inst[11:7];
    assign funct3 = inst[14:12];
    assign rs1    = inst[19:15];
    assign rs2    = inst[24:20];

    wire   op_reg   = (opcode == 7'h33); // 寄存器-寄存器
    wire   op_imm   = (opcode == 7'h13); // 寄存器-立即数
    assign is_lui   = (opcode == 7'h37); // 高位加载
    wire   is_auipc = (opcode == 7'h17); // 高位地址
    assign is_jalr  = (opcode == 7'h67); // 间接跳转
    assign is_load  = (opcode == 7'h03); // 加载
    wire   is_store = (opcode == 7'h23); // 存储

    assign is_lw  = is_load  && (funct3 == 3'b010); 
    assign is_lbu = is_load  && (funct3 == 3'b100); 
    assign is_sw  = is_store && (funct3 == 3'b010); 
    assign is_sb  = is_store && (funct3 == 3'b000); 
 
    assign wen = op_reg | op_imm | is_load | is_lui | is_auipc| is_jalr;
    assign alu_op = is_auipc ? 4'b1111 : ((op_reg) ? {inst[30], funct3} : {1'b0, funct3});
    assign alu_src = ~op_reg;

    wire [31:0] i_imm = {{20{inst[31]}}, inst[31:20]}; // I-type
    wire [31:0] s_imm = {{20{inst[31]}}, inst[31:25], inst[11:7]}; // S-type

    assign u_imm = {inst[31:12], 12'b0}; // U-type
    assign imm32 = is_store ? s_imm :
        (is_lui | is_auipc) ? u_imm :
                              i_imm;
    
endmodule