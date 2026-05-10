// vsrc/core/memory/WriteMask.v
module WriteMask (
    input        is_sw, is_sh, is_sb,
    input [1:0]  addr_offset,
    output reg [7:0] mem_wmask
);
    always @(*) begin
        if (is_sw) begin
            mem_wmask = 8'b1111;
        end
        else if (is_sh) begin
            case (addr_offset)
                2'b00:   mem_wmask = 8'b0011;
                2'b10:   mem_wmask = 8'b1100;
                default: mem_wmask = 8'b0000;
            endcase
        end 
        else if (is_sb) begin
            case (addr_offset)
                2'b00:   mem_wmask = 8'b0001;
                2'b01:   mem_wmask = 8'b0010;
                2'b10:   mem_wmask = 8'b0100;
                2'b11:   mem_wmask = 8'b1000;
                default: mem_wmask = 8'b0000;
            endcase
        end 
        else begin
            mem_wmask = 8'b0000;
        end
    end
endmodule