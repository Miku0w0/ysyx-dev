module Execute (
    input  [31:0] rdata1, rdata2,
    input  [31:0] imm32,
    input  [3:0]  alu_op,
    input         alu_src,
    input  [2:0]  funct3,
    
    output [31:0] alu_res,
    output [31:0] jump_target,
    output [1:0]  alu_res_1_0,
    output [23:0] alu_res_25_2,
    output [3:0]  alu_res_31_28
);

    wire [31:0] src2 = alu_src ? imm32 : rdata2;

    reg [31:0] alu_out;
    always @(*) begin
        case (alu_op)
            4'b0000: alu_out = rdata1 + src2; 
            4'b1000: alu_out = rdata1 - src2; 
            4'b0001: alu_out = rdata1 << src2[4:0]; 
            
            default: alu_out = rdata1 + src2;
        endcase
    end
    assign alu_res = alu_out;

    // 计算跳转地址
    assign jump_target = alu_res & 32'hfffffffe;

    // 计算结果用于各类信号
    assign alu_res_1_0   = alu_res[1:0];
    assign alu_res_25_2  = alu_res[25:2];
    assign alu_res_31_28 = alu_res[31:28];
endmodule