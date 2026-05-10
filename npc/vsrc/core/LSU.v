// vsrc/core/LSU.v
module LSU (
    input         clk,
    input  [31:0] alu_res,
    input  [31:0] rdata2,
    input         is_sw, is_sb, is_sh,
    input         is_lw, is_lh, is_lb, is_lhu, is_lbu,
    
    output        mem_we,
    output [7:0]  mem_wmask,
    output [31:0] mem_rdata_o,
    output [31:0] mem_wdata_i
);

    /* 地址对齐 */
    wire [31:0] aligned_addr;
    wire [1:0]  addr_offset;
    
    AddrAlign u_addralign (
        .alu_res(alu_res),

        .aligned_addr(aligned_addr),
        .addr_offset(addr_offset)
    );
    
    /* 写控制 */
    WriteCtrl u_writectrl (
        .is_sw(is_sw),
        .is_sh(is_sh),
        .is_sb(is_sb),
        .rdata2(rdata2),

        .mem_we(mem_we),
        .mem_wdata_i(mem_wdata_i)
    );
    
    /* 写掩码 */
    WriteMask u_writemask (
        .is_sw(is_sw),
        .is_sh(is_sh),
        .is_sb(is_sb),
        .addr_offset(addr_offset),

        .mem_wmask(mem_wmask)
    );
    
    /* 写执行 */
    WriteExecute u_writeexecute (
        .clk(clk),
        .mem_we(mem_we),
        .aligned_addr(aligned_addr),
        .mem_wdata_i(mem_wdata_i),
        .mem_wmask(mem_wmask)
    );
    
    /* 读控制 */
    wire        is_load;
    wire [31:0] mem_rdata_raw;
    ReadCtrl u_readctrl (
        .is_lw(is_lw),
        .is_lh(is_lh),
        .is_lb(is_lb),
        .is_lhu(is_lhu),
        .is_lbu(is_lbu),
        .aligned_addr(aligned_addr),

        .is_load(is_load),
        .mem_rdata_raw(mem_rdata_raw)
    );
    
    /* 读拓展 */
    ReadExtend u_readextend (
        .is_load(is_load),
        .is_lw(is_lw),
        .is_lh(is_lh),
        .is_lb(is_lb),
        .is_lhu(is_lhu),
        .is_lbu(is_lbu),
        .addr_offset(addr_offset),
        .mem_rdata_raw(mem_rdata_raw),
        
        .mem_rdata_o(mem_rdata_o)
    );

endmodule