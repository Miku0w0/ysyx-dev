module MemoryAccess (
    /* from Ex */
    input  [31:0] alu_res,      // 作为访存地址
    input  [3:0]  addr_high,    // alures高4位
    input  [1:0]  addr_offset,  // alures低2位 用于选择byte/half
    /* from ID */
    input         is_sw, is_sb, is_sh,  
    input         is_lw, is_lh, is_lb, is_lhu, is_lbu,
    /* from rf */
    input  [31:0] rdata2,       // store时写入RAM的数据，用于写入逻辑
    /* from RAM */
    input  [31:0] ram_o_raw,    // RAM读出的原始32位数据，用于读回逻辑

    /* to WB 读回逻辑 */
    output [31:0] mem_rdata_out,// load时从RAM读出的数据 写回寄存器
    /* to RAM 写入逻辑 */
    output [31:0] ram_addr_i,   // 访问地址 alures
    output [31:0] ram_data_i,   // 写入数据 rdata2
    output        ram_we,       // 写使能    
    output [3:0]  ram_wmask     // 写掩码（按字节）
);

    // ===== 写入逻辑 =====
    assign ram_addr_i = alu_res;                                
    // 在RAM里用于选择有效字节写入 且在RAM里由ram_wmask字节掩码控制写入
    assign ram_data_i = is_sb ? {4{rdata2[7:0]}} :
                        is_sh ? {2{rdata2[15:0]}} : rdata2;
    assign ram_we     = (is_sw | is_sb | is_sh);                    
    /* 字节掩码生成 */
    // 单字节掩码 写入字节，8位
    wire [3:0] sb_mask = (addr_offset == 2'd0) ? 4'b0001 :   // 00对应0001
                         (addr_offset == 2'd1) ? 4'b0010 :   // 01对应0010
                         (addr_offset == 2'd2) ? 4'b0100 :   // 10对应0100
                                                 4'b1000;    // 11对应1000
    // 半字掩码 写入半字，16位
    wire [3:0] sh_mask = (addr_offset == 2'd0) ? 4'b0011 :   // 00对应0011
                         (addr_offset == 2'd2) ? 4'b1100 :   // 10对应1100
                                                 4'b0000; 
    // 最终字节掩码生成
    assign ram_wmask = is_sw ? 4'b1111 :                     // 4字节
                       is_sh ? sh_mask :                     // 2字节
                       is_sb ? sb_mask :                     // 1字节
                               4'b0000;

    // ===== 读回逻辑 =====
    wire [7:0] selected_byte = (addr_offset == 2'b00) ? ram_o_raw[7:0]   :  // 00 7-0位
                               (addr_offset == 2'b01) ? ram_o_raw[15:8]  :  // 01 15-8位
                               (addr_offset == 2'b10) ? ram_o_raw[23:16] :  // 10 23-16位
                                                        ram_o_raw[31:24];   // 11 31-24位

    wire [15:0] selected_half = (addr_offset == 2'd0) ? ram_o_raw[15:0]  :  // 00 15-0位
                                (addr_offset == 2'd2) ? ram_o_raw[31:16] :  // 10 31-16位
                                                        16'h0000;           // 01/11 非法情况
    
    assign mem_rdata_out = is_lb  ? {{24{selected_byte[7]}}, selected_byte} : // 字节有符号拓展
                           is_lbu ? {24'b0, selected_byte} :                  // 字节无符号拓展
                           is_lh  ? {{16{selected_half[15]}}, selected_half} :// 半字有符号拓展
                           is_lhu ? {16'b0, selected_half} :                  // 半字无符号拓展
                           is_lw  ? ram_o_raw :                               // 全字
                                    32'hdeadbeef;                             // 非法情况

endmodule