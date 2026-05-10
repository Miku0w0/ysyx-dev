// vsrc/core/decode/Decoder.v
module Decoder (
    input  [6:0] opcode,
    
    // 指令大类
    output reg is_lui,
    output reg is_auipc,
    output reg is_jal,
    output reg is_jalr,
    output reg is_branch,
    output reg is_load,
    output reg is_store,
    
    // 辅助信号
    output reg is_r_type,
    output reg is_i_type
);

    always @(*) begin
        // 默认值
        is_lui    = 1'b0;
        is_auipc  = 1'b0;
        is_jal    = 1'b0;
        is_jalr   = 1'b0;
        is_branch = 1'b0;
        is_load   = 1'b0;
        is_store  = 1'b0;
        is_r_type = 1'b0;
        is_i_type = 1'b0;
        
        case (opcode)
            7'h37: is_lui    = 1'b1;  // lui
            7'h17: is_auipc  = 1'b1;  // auipc
            7'h6f: is_jal    = 1'b1;  // jal
            7'h67: is_jalr   = 1'b1;  // jalr
            7'h63: is_branch = 1'b1;  // branch
            7'h03: is_load   = 1'b1;  // load
            7'h23: is_store  = 1'b1;  // store
            7'h33: is_r_type = 1'b1;  // R-type
            7'h13: is_i_type = 1'b1;  // I-type
            default: ;  // 其他指令
        endcase
    end

endmodule