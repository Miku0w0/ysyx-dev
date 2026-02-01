module Reg #(WIDTH = 32, RESET_VAL = 32'h80000000) (
    input clk,
    input rst,
    input [WIDTH-1:0] din,    // snpc
    output reg [WIDTH-1:0] dout, // pc
    input wen                 // 写使能
);
    always @(posedge clk) begin
        if (rst) begin
            // 复位时，回到起始地址
            dout <= RESET_VAL;
        end
        else if (wen) begin
            // 只有使能有效，才在时钟上升沿存入新值
            dout <= din;
        end
    end
endmodule