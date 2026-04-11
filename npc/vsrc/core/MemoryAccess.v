module MemoryAccess (
    input  [23:0] alu_res_25_2,
    input  [3:0]  alu_res_31_28,
    input  [1:0]  alu_res_1_0,
    input         is_sw, is_sb,
    input  [31:0] rdata2,

    output [23:0] ram_addr_i,
    output [31:0] ram_data_i,
    output        M3, M2, M7, M6, M5, M4
);

    // 地址映射
    assign ram_addr_i = alu_res_25_2;

    // 控制信号 M3 M2 (片选/读写控制)
    //assign M3 = (alu_res_31_28 >= 4'd2) && (is_sw | is_sb);
    assign M3 = (is_sw | is_sb);
    assign M2 = 1'b1;

    // 字节写使能信号 M4-M7
    wire [3:0] res_decd;
    assign res_decd = is_sb ? (4'b0001 << alu_res_1_0) : 4'b0000;
    assign M7 = res_decd[3] | is_sw;
    assign M6 = res_decd[2] | is_sw;
    assign M5 = res_decd[1] | is_sw;
    assign M4 = res_decd[0] | is_sw;

    // 写入数据对齐
    wire [31:0] rdata2_7_0_32 = {24'b0, rdata2[7:0]};
    wire [31:0] rdata2_shift;

    assign rdata2_shift = (alu_res_1_0 == 2'b00) ? rdata2_7_0_32 :
                          (alu_res_1_0 == 2'b01) ? (rdata2_7_0_32 << 8) :
                          (alu_res_1_0 == 2'b10) ? (rdata2_7_0_32 << 16) :
                                                   (rdata2_7_0_32 << 24);

    assign ram_data_i = is_sb ? rdata2_shift : rdata2;

endmodule