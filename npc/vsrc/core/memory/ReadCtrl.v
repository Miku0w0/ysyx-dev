// vsrc/core/memory/ReadCtrl.v
module ReadCtrl (
    input        is_lw, is_lh, is_lb, is_lhu, is_lbu,
    input  [31:0] aligned_addr,
    output       is_load,
    output reg [31:0] mem_rdata_raw
);
    assign is_load = is_lw | is_lh | is_lb | is_lhu | is_lbu;
    
    always @(*) begin
        if (is_load) begin
            mem_rdata_raw = pmem_read(aligned_addr);
        end
        else begin
            mem_rdata_raw = 32'h0;
        end
    end
endmodule