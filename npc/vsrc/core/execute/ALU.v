// vsrc/core/execute/ALU.v
module ALU (
    input [31:0] src1, src2,
    input [3:0] alu_op,
    output reg [31:0] alu_res
);
    always @(*) begin
        case (alu_op)
            4'b0000: alu_res = src1 + src2;
            4'b1000: alu_res = src1 - src2;
            4'b0001: alu_res = src1 << src2[4:0];
            4'b0100: alu_res = src1 ^ src2;
            4'b0110: alu_res = src1 | src2;
            4'b0111: alu_res = src1 & src2;
            4'b0010: alu_res = ($signed(src1) < $signed(src2)) ? 1 : 0;
            4'b0011: alu_res = (src1 < src2) ? 1 : 0;
            4'b0101: alu_res = src1 >> src2[4:0];
            4'b1101: alu_res = $signed(src1) >>> src2[4:0];
            default: alu_res = src1 + src2;
        endcase
    end
endmodule