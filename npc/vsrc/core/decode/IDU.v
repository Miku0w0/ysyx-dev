module IDU (
    /* from IFU */
    input  [31:0] inst,

    /* 固定字段 */
    output [4:0]  waddr, rs1, rs2, // 寄存器索引
    output [2:0]  funct3,          // 子操作码
    output [31:0] u_imm, imm32,    // 立即数
    /* 控制信号 */
    output        wen,             // 写使能 
    output        alu_src,         // alu来源
    output [3:0]  alu_op,          // alu类型
    /* 指令类型 */
    output        is_lui, is_auipc, 
    output        is_jalr, is_jal, is_branch,
    output        is_store,is_sw, is_sb, is_sh,
    output        is_load, is_lw, is_lb, is_lbu, is_lh, is_lhu
);
    /* 指令字段提取 */
    wire [6:0] opcode = inst[6:0];  
    assign waddr  = inst[11:7];    
    assign funct3 = inst[14:12];    
    assign rs1    = inst[19:15];    
    assign rs2    = inst[24:20];    

    /* 指令大类识别 */
    wire is_r_type = (opcode == 7'h33);
    wire is_i_type = (opcode == 7'h13);
    assign is_lui    = (opcode == 7'h37); 
    assign is_auipc  = (opcode == 7'h17); 
    assign is_jalr   = (opcode == 7'h67); 
    assign is_jal    = (opcode == 7'h6f); 
    assign is_branch = (opcode == 7'h63); 
    assign is_load   = (opcode == 7'h03); 
    assign is_store  = (opcode == 7'h23);
    
    /* 访存细分 */
    assign is_lb  = is_load  && (funct3 == 3'b000);
    assign is_lh  = is_load  && (funct3 == 3'b001); 
    assign is_lw  = is_load  && (funct3 == 3'b010);
    assign is_lbu = is_load  && (funct3 == 3'b100); 
    assign is_lhu = is_load  && (funct3 == 3'b101);
    assign is_sb  = is_store && (funct3 == 3'b000); 
    assign is_sh  = is_store && (funct3 == 3'b001);
    assign is_sw  = is_store && (funct3 == 3'b010); 

    /* 控制信号 */
    assign alu_src = ~is_r_type;
    assign wen     = ~(is_store | is_branch);
    /* alu操作类型 */
    reg [3:0] alu_op_out;
    always @(*) begin
        if (is_load | is_store | is_auipc) alu_op_out = 4'b0000;
        else if (is_r_type) begin
            if (funct3 == 3'b101 && inst[30]) alu_op_out = 4'b1101;  // SRA
            else alu_op_out = {inst[30], funct3};
        end
        else if (is_i_type) begin
            if (funct3 == 3'b101 && inst[30]) alu_op_out = 4'b1101;  // SRAI
            else alu_op_out = {1'b0, funct3};
        end
        else alu_op_out = {1'b0, funct3};
        
    end
    assign alu_op = alu_op_out;

    /* 立即数拼接 */
    wire [31:0] i_imm = {{20{inst[31]}}, inst[31:20]};
    wire [31:0] s_imm = {{20{inst[31]}}, inst[31:25], inst[11:7]};         
    wire [31:0] b_imm = {{20{inst[31]}}, inst[7], inst[30:25], inst[11:8], 1'b0}; 
    wire [31:0] j_imm = {{12{inst[31]}}, inst[19:12], inst[20], inst[30:21], 1'b0};   
    assign u_imm = {inst[31:12], 12'b0}; 

    /* 立即数选择 */                                           
    reg [31:0] imm32_out;
    always @(*) begin
        if (is_store)               imm32_out = s_imm;
        else if (is_branch)         imm32_out = b_imm;
        else if (is_lui | is_auipc) imm32_out = u_imm;
        else if (is_jal)            imm32_out = j_imm;
        else                        imm32_out = i_imm;
    end
    assign imm32 = imm32_out;

    
endmodule