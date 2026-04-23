/* verilator lint_off WIDTHEXPAND */
module RAM (
    input         clk,
    /* from LSU */
    input  [31:0] ram_addr_i,   // 访问地址 alures
    input  [31:0] ram_data_i,   // 写入数据 rdata2
    input  [3:0]   ram_wmask,   // 写掩码 由字节控制
    input          ram_we,      // 写使能 store为1

    output [31:0] ram_data_o       // RAM读取的数据
);

    assign ram_data_o  = pmem_read(ram_addr_i);

    // ===== 写入ram ======
    always @(posedge clk) begin //ram_wmask 控制 byte 写入
        if (ram_we) begin
            pmem_write(ram_addr_i, ram_data_i, {4'b0, ram_wmask});
        end
    end

endmodule