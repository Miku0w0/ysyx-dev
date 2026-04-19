module Reg #(WIDTH = 32, RESET_VAL = 32'h80000000) (
    input clk,
    input rst,
    input [WIDTH-1:0] din, 
    input wen,
    output reg [WIDTH-1:0] dout
);
    always @(posedge clk) begin
        if (rst) begin
            dout <= RESET_VAL;  // 复位回到起始地址
        end
        else if (wen) begin
            dout <= din;        // 使能有效存入新值dnpc
        end
    end
endmodule