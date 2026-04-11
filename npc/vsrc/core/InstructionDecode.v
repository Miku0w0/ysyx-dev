module InstructionDecode (
    input  [31:0] inst,

    // 固定信号
    output [4:0]  rd,
    output [4:0]  rs1,
    output [4:0]  rs2,

    // 控制信号
    output [2:0]  funct3,
    output        alu_src,
    output        wen,
    output [31:0] u_imm,
    output [31:0] imm32,

    // 指令类型
    output        is_load,
    output        is_lw,
    output        is_lbu,
    output        is_sw,
    output        is_sb,
    output        is_lui,
    output        is_jalr
);
    // 指令拆解
    wire [6:0] opcode = inst[6:0];
    assign rd     = inst[11:7];
    assign funct3 = inst[14:12];
    assign rs1    = inst[19:15];
    assign rs2    = inst[24:20];

    // 指令类型
    wire   op_reg   = (opcode == 7'h33); // 寄存器-寄存器
    wire   op_imm   = (opcode == 7'h13); // 寄存器-立即数
    assign is_load  = (opcode == 7'h03); // 加载
    assign is_lui   = (opcode == 7'h37); // 高位加载
    wire   is_store = (opcode == 7'h23); // 存储
    assign is_jalr  = (opcode == 7'h67); // 间接跳转

    // Load/Store类型细分
    assign is_lw  = is_load  && (funct3 == 3'b010); 
    assign is_lbu = is_load  && (funct3 == 3'b100); 
    assign is_sw  = is_store && (funct3 == 3'b010); 
    assign is_sb  = is_store && (funct3 == 3'b000); 

    // 数据源选择和写使能 
    assign alu_src = ~op_reg;
    assign wen = op_reg | op_imm | is_load | is_lui | is_jalr;

    // 立即数处理
    wire [31:0] i_imm = {{20{inst[31]}}, inst[31:20]}; // I-type
    wire [31:0] s_imm = {{20{inst[31]}}, inst[31:25], inst[11:7]}; // S-type

    // imm32选择
    assign imm32 = is_store ? s_imm : i_imm;
    assign u_imm = {inst[31:12], 12'b0}; // U-type
    
endmodule