// vsrc/core/memory/AddrAlign.v
module AddrAlign (
    input  [31:0] alu_res,
    output [31:0] aligned_addr,
    output [1:0]  addr_offset
);
    assign aligned_addr = {alu_res[31:2], 2'b0}; // 基地址
    assign addr_offset  = alu_res[1:0];          // 偏移量
endmodule