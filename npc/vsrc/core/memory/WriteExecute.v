// vsrc/core/memory/WriteExecute.v
module WriteExecute (
    input         clk,
    input         mem_we,
    input  [31:0] aligned_addr,
    input  [31:0] mem_wdata_i,
    input  [7:0]  mem_wmask
);
    always @(posedge clk) begin
        if (mem_we) begin
            pmem_write(aligned_addr, mem_wdata_i, mem_wmask);
        end
    end
endmodule