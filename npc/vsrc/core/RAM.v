/* verilator lint_off WIDTHEXPAND */
module RAM (
    input         clk,
    input  [23:0] ram_addr_i, // 地址
    input  [31:0] ram_data_i, // 写入数据
    input         M3,         // 片选
    input         M2,         // 读写
    input         M4, M5, M6, M7, // Byte Enable 0-3
    output [31:0] ram_o       // 读取数据
);
    // 定义存储阵列
    reg [31:0] mem [0:16777215];

    // 读取逻辑
    assign ram_o = mem[ram_addr_i];

    // 写入逻辑(按字节)
    always @(posedge clk) begin
        if (M3) begin
            if (M4) mem[ram_addr_i][7:0]   <= ram_data_i[7:0];
            if (M5) mem[ram_addr_i][15:8]  <= ram_data_i[15:8];
            if (M6) mem[ram_addr_i][23:16] <= ram_data_i[23:16];
            if (M7) mem[ram_addr_i][31:24] <= ram_data_i[31:24];
        end
    end

endmodule