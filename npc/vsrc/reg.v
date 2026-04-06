module reg #(WIDTH = 32, RESET_VAL = 32'h80000000) (
    input clk,
    input rst,
    input [WIDTH-1:0] din, 
    output reg [WIDTH-1:0] dout, 
    input wen 
);
    always @(posedge clk) begin
        if (rst) begin
            dout <= RESET_VAL; // 复位时，回到起始地址
        end
        else if (wen) begin
            dout <= din; // 只有使能有效，才在时钟上升沿存入新值
        end
    end
endmodule