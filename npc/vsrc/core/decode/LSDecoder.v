// vsrc/core/decode/LSDecoder.v
module LSDecoder (
    input        is_load,
    input        is_store,
    input [2:0]  funct3,
    
    output reg is_lb,
    output reg is_lh,
    output reg is_lw,
    output reg is_lbu,
    output reg is_lhu,
    output reg is_sb,
    output reg is_sh,
    output reg is_sw
);
    always @(*) begin
        // 默认值
        is_lb = 1'b0; is_lh = 1'b0; is_lw = 1'b0;
        is_lbu = 1'b0; is_lhu = 1'b0;
        is_sb = 1'b0; is_sh = 1'b0; is_sw = 1'b0;
        
        if (is_load) begin
            case (funct3)
                3'b000: is_lb = 1'b1;
                3'b001: is_lh = 1'b1;
                3'b010: is_lw = 1'b1;
                3'b100: is_lbu = 1'b1;
                3'b101: is_lhu = 1'b1;
                default: ;
            endcase
        end
        
        if (is_store) begin
            case (funct3)
                3'b000: is_sb = 1'b1;
                3'b001: is_sh = 1'b1;
                3'b010: is_sw = 1'b1;
                default: ;
            endcase
        end
    end
endmodule