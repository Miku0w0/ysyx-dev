module RegFile #(ADDR_WIDTH = 1, DATA_WIDTH = 1) (
  input clk,
  input [DATA_WIDTH-1:0] wdata,
  input [ADDR_WIDTH-1:0] waddr,
  input wen,
  input [ADDR_WIDTH-1:0] raddr1,
  output [DATA_WIDTH-1:0] rdata1,
  output [31:0] x1_val
    
);
  reg [DATA_WIDTH-1:0] rf [2**ADDR_WIDTH-1:0]; // 寄存器数据宽度 寄存器个数
  always @(posedge clk) begin // x0不能被写入
    if (wen && waddr != 5'b0) rf[waddr] <= wdata;
  end

  assign rdata1 = (raddr1 == 5'b0 ) ? {DATA_WIDTH{1'b0}} : rf[raddr1]; // DATA_WIDTH=32个0拼接 raddr1对应寄存器数值
  assign x1_val = rf[1];

endmodule