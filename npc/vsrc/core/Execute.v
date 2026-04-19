module Execute (
    /* from IF */
    input  [31:0] pc_i,        // auipc/jalr
    /* from rf */
    input  [31:0] rdata1,
    input  [31:0] rdata2,
    /* from ID */
    input  [31:0] imm32,
    input  [3:0]  alu_op,
    input         alu_src,
    input  [2:0]  funct3,
    
    output [31:0] jump_target, // 跳转目标地址
    output [31:0] alu_res
);
    /* 操作数选择 */
    wire [31:0] src2 = alu_src ? imm32 : rdata2; // I imm32 R rdata2
    /* alu计算 */
    reg [31:0] alu_out;
    always @(*) begin
        case (alu_op)
            4'b0000: alu_out = rdata1 + src2; // add/addi/load/store
            4'b1000: alu_out = rdata1 - src2; // sub
            4'b0001: alu_out = rdata1 << src2[4:0]; // sll(shift logic)
            4'b0100: alu_out = rdata1 ^ src2; // xor         
            4'b0110: alu_out = rdata1 | src2; // or    
            4'b0111: alu_out = rdata1 & src2; // and     
            4'b0010: alu_out = ($signed(rdata1) < $signed(src2)) ? 32'd1 : 32'd0; // slt signed less
            4'b0011: alu_out = (rdata1 < src2) ? 32'd1 : 32'd0; //sltu
            4'b0101: alu_out = rdata1 >> src2[4:0]; // srl     
            4'b1101: alu_out = $signed(rdata1) >>> src2[4:0]; // sra 算术（负数）右移 复制符号位 左移不会破坏符号位
            4'b1111: alu_out = pc_i + src2; // auipc jalr地址
            default: alu_out = rdata1 + src2; // 加法
        endcase
    end

    assign alu_res = alu_out;
    assign jump_target = alu_res & 32'hfffffffe; // 计算跳转地址
endmodule