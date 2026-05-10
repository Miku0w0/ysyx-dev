// vsrc/core/decode/CtrlGen.v
module CtrlGen (
    input  [31:0] inst,
    input         is_r_type,
    input         is_i_type,
    input         is_load,
    input         is_store,
    input         is_auipc,
    input         is_branch,
    input  [2:0]  funct3,
    
    output        alu_src,
    output        wen,
    output [3:0]  alu_op
);

    // ALU源操作数选择
    assign alu_src = ~is_r_type;  // R-type: 来自寄存器, 其他: 来自立即数
    
    // 寄存器写使能 (store和branch不写回)
    assign wen = ~(is_store | is_branch);
    
    // ALU操作码生成
    reg [3:0] alu_op_out;
    always @(*) begin
        if (is_load | is_store | is_auipc) begin
            alu_op_out = 4'b0000;  // add/addi
        end
        else if (is_r_type) begin
            if (funct3 == 3'b101 && inst[30]) begin
                alu_op_out = 4'b1101;  // SRA
            end
            else begin
                alu_op_out = {inst[30], funct3};  // R-type: inst[30] + funct3
            end
        end
        else if (is_i_type) begin
            if (funct3 == 3'b101 && inst[30]) begin
                alu_op_out = 4'b1101;  // SRAI
            end
            else begin
                alu_op_out = {1'b0, funct3};  // I-type: 0 + funct3
            end
        end
        else begin
            alu_op_out = {1'b0, funct3};  // 默认
        end
    end
    assign alu_op = alu_op_out;

endmodule