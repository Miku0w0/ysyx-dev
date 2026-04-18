module MemoryAccess (
    input  [31:0] addr_word,
    input  [3:0]  addr_high, // mmio
    input  [1:0]  addr_offset,
    // 读写信号
    input         is_sw, is_sb, is_sh,  
    input         is_lw, is_lh, is_lb, is_lhu, is_lbu,
    input  [31:0] rdata2,        
    // 读回处理后的数据
    input  [31:0] ram_o_raw,     // RAM 吐出的原始 32 位数据
    output [31:0] mem_rdata_out, // 加工后的写回数据 (Load Data)
    // 连RAM
    output [31:0] ram_addr_i,
    output [31:0] ram_data_i,
    output        ram_we,       
    output [3:0]  ram_wmask     
);

    // --写入逻辑--
    assign ram_addr_i = addr_word;
    assign ram_we     = (is_sw | is_sb | is_sh);

    // 字节掩码生成
    wire [3:0] sb_mask = (addr_offset == 2'd0) ? 4'b0001 :
                         (addr_offset == 2'd1) ? 4'b0010 :
                         (addr_offset == 2'd2) ? 4'b0100 : 4'b1000;

    wire [3:0] sh_mask = (addr_offset == 2'd0) ? 4'b0011 :
                         (addr_offset == 2'd2) ? 4'b1100 : 4'b0000;

    assign ram_wmask = is_sw ? 4'b1111 : 
                       is_sh ? sh_mask : 
                       is_sb ? sb_mask : 4'b0000;

    // 数据广播
    assign ram_data_i = is_sb ? {4{rdata2[7:0]}} :
                        is_sh ? {2{rdata2[15:0]}} : rdata2;

    // --读回逻辑--
    wire [7:0] selected_byte = (addr_offset == 2'b00) ? ram_o_raw[7:0]   :
                               (addr_offset == 2'b01) ? ram_o_raw[15:8]  :
                               (addr_offset == 2'b10) ? ram_o_raw[23:16] : 
                                                        ram_o_raw[31:24];

    wire [15:0] selected_half = addr_offset[1] ? ram_o_raw[31:16] : ram_o_raw[15:0];
    
    assign mem_rdata_out = is_lb  ? {{24{selected_byte[7]}}, selected_byte} :
                           is_lbu ? {24'b0, selected_byte} :
                           is_lh  ? {{16{selected_half[15]}}, selected_half} :
                           is_lhu ? {16'b0, selected_half} :
                           is_lw  ? ram_o_raw : 32'hdeadbeef;

endmodule