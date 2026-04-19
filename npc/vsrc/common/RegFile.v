module RegFile #(ADDR_WIDTH = 1, DATA_WIDTH = 1) (
  /* from top */
  input clk,
  input reset,
  /* from WB */
  input [DATA_WIDTH-1:0] wdata,
  /* from ID */
  input [ADDR_WIDTH-1:0] waddr,
  input wen,
  input [ADDR_WIDTH-1:0] rs1,
  input [ADDR_WIDTH-1:0] rs2,
  
  output [DATA_WIDTH-1:0] rdata1,
  output [DATA_WIDTH-1:0] rdata2
);
  reg [DATA_WIDTH-1:0] rf [2**ADDR_WIDTH-1:0]; // 数据宽度 个数
  always @(posedge clk) begin // x0
    if (wen && waddr != 5'b0) rf[waddr] <= wdata;
  end

  assign rdata1 = (rs1 == 5'b0 ) ? {DATA_WIDTH{1'b0}} : rf[rs1];
  assign rdata2 = (rs2 == 5'b0 ) ? {DATA_WIDTH{1'b0}} : rf[rs2]; 


endmodule