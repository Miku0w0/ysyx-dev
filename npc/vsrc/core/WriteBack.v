module WriteBack(
    input  [1:0]  alu_res_1_0,   // 用于字节选择的地址低2位
    input  [31:0] alu_res,       // ALU 计算结果
    input  [31:0] ram_o,         // 从内存读取的32位数据
    input  [31:0] snpc,          
    input  [31:0] u_imm,         
    input         is_load,       
    input         is_lw,         
    input         is_lbu,        
    input         is_lui,        
    input         is_jalr,       

    output [31:0] wdata_i        // 最终写回寄存器的地址
);

    // lbu 根据地址低两位 选出对应的字节
    wire [7:0] ram_o_8;
    assign ram_o_8 = (alu_res_1_0 == 2'b00) ? ram_o[7:0]   :
                     (alu_res_1_0 == 2'b01) ? ram_o[15:8]  :
                     (alu_res_1_0 == 2'b10) ? ram_o[23:16] :
                                              ram_o[31:24];

    // Load: lbu 零扩展 lw 内存输出
    wire [31:0] ram_o_final;
    assign ram_o_final = is_lbu ? {24'b0, ram_o_8} : 
                         is_lw  ? ram_o            : 
                                  32'b0;

    // 决定wdata_i
    wire [1:0] sel;
    wire jalr_load = is_jalr | is_load;
    wire jalr_lui  = is_jalr | is_lui;
    assign sel = {jalr_lui, jalr_load};

    // 选择ram_o_final alu_res u_imm snpc
    assign wdata_i = (sel == 2'b01) ? ram_o_final : // Load 指令写回
                     (sel == 2'b00) ? alu_res     : // 普通 ALU 指令写回
                     (sel == 2'b11) ? snpc        : // 跳转指令写回 (保存 PC+4)
                     (sel == 2'b10) ? u_imm       : // lui 指令写回
                                      32'b0;                               

endmodule