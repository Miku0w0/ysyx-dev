// vsrc/core/decode/ImmGen.v
module ImmGen (
    input  [31:0] inst,
    input         is_store,
    input         is_branch,
    input         is_lui_auipc,
    input         is_jal,
    
    output [31:0] imm32,
    output [31:0] u_imm
);

    // 各种立即数格式
    wire [31:0] i_imm = {{20{inst[31]}}, inst[31:20]};
    wire [31:0] s_imm = {{20{inst[31]}}, inst[31:25], inst[11:7]};
    wire [31:0] b_imm = {{20{inst[31]}}, inst[7], inst[30:25], inst[11:8], 1'b0};
    wire [31:0] j_imm = {{12{inst[31]}}, inst[19:12], inst[20], inst[30:21], 1'b0};
    
    // U型立即数（高20位）
    assign u_imm = {inst[31:12], 12'b0};
    
    // 立即数选择
    reg [31:0] imm32_out;
    always @(*) begin
        if (is_store)           imm32_out = s_imm;
        else if (is_branch)     imm32_out = b_imm;
        else if (is_lui_auipc)  imm32_out = u_imm;
        else if (is_jal)        imm32_out = j_imm;
        else                    imm32_out = i_imm;  // I-type or default
    end
    
    assign imm32 = imm32_out;

endmodule