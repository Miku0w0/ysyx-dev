/* verilator lint_off WIDTHEXPAND */
module RAM (
    input         clk,
    /* from Mem */
    input  [31:0] ram_addr_i,   // 访问地址 alures
    input  [31:0] ram_data_i,   // 写入数据 rdata2
    input  [3:0]   ram_wmask,   // 写掩码 由字节控制
    input          ram_we,      // 写使能 store为1

    
    output [31:0] ram_o_raw       // RAM读取的数据
);
    // 16M 2^24 ram
    reg [31:0] mem [0:16777215];
    // 地址转换 字节到字
    wire [23:0] word_idx = ram_addr_i[25:2]; // alures[25:2]
    // 初始化ram
    integer i;
    initial begin
        for (i = 0; i < 16777216; i = i + 1)
            mem[i] = 32'h00000000;
    end
    // ===== 读取ram =====
    assign ram_o_raw  = mem[word_idx];

    // ===== 写入ram ======
    always @(posedge clk) begin //ram_wmask 控制 byte 写入
        if (ram_we) begin
            if (ram_wmask[0]) mem[word_idx][7:0]   <= ram_data_i[7:0];
            if (ram_wmask[1]) mem[word_idx][15:8]  <= ram_data_i[15:8];
            if (ram_wmask[2]) mem[word_idx][23:16] <= ram_data_i[23:16];
            if (ram_wmask[3]) mem[word_idx][31:24] <= ram_data_i[31:24];
            
        end
    end

endmodule