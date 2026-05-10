// vsrc/core/memory/WriteCtrl.v
module WriteCtrl (
    input        is_sw, is_sh, is_sb,
    input [31:0] rdata2,
    output       mem_we,
    output reg [31:0] mem_wdata_i
);
    assign mem_we = (is_sw | is_sh | is_sb);
    
    always @(*) begin
        if (is_sb)      mem_wdata_i = {4{rdata2[7:0]}};
        else if (is_sh) mem_wdata_i = {2{rdata2[15:0]}};
        else            mem_wdata_i = rdata2;
    end
endmodule