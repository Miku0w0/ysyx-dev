module Write_Back(
    input [31:0] alu_res,    // 来自 ALU 的计算结果
    input [31:0] snpc,       // 来自 PC 模块的 PC + 4
    input [31:0] mem_rdata,  // 如果后续实现 Load 指令，需要从内存读数据
    input [1:0]  wb_sel,     // 由 Decoder 产生的选择信号
    output [31:0] wb_data    // 最终写回 RegFile 的数据
);

    // 根据选择信号决定写回内容
    // 00: ALU 结果 (addi 等)
    // 01: PC + 4 (jal, jalr)
    // 10: 内存数据 (ld, lw 等)
    assign wb_data = (wb_sel == 2'b00) ? alu_res :
                     (wb_sel == 2'b01) ? snpc    :
                     (wb_sel == 2'b10) ? mem_rdata : 32'b0;

endmodule