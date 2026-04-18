/* verilator lint_off WIDTHEXPAND */
module RAM (
    input         clk,
    input  [23:0] ram_addr_i, // 地址
    input  [31:0] ram_data_i, // 写入数据
    input  [3:0]   ram_wmask,
    input          ram_we,

    output [31:0] mem_rdata_raw       // 读取数据
);
    // 定义存储阵列
    reg [31:0] mem [0:16777215];

    wire [21:0] word_idx = ram_addr_i[23:2];
    // 读取逻辑
    assign mem_rdata_raw = mem[word_idx];

    // 写入逻辑(按字节)
    always @(posedge clk) begin
        if (ram_we) begin
            // ram_wmask[0] 对应原来的 M4, [1] 对应 M5, 以此类推
            if (ram_wmask[0]) mem[word_idx][7:0]   <= ram_data_i[7:0];
            if (ram_wmask[1]) mem[word_idx][15:8]  <= ram_data_i[15:8];
            if (ram_wmask[2]) mem[word_idx][23:16] <= ram_data_i[23:16];
            if (ram_wmask[3]) mem[word_idx][31:24] <= ram_data_i[31:24];
        end
    end

endmodule