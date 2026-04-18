/* verilator lint_off WIDTHEXPAND */
module ROM (
    input  [23:0] rom_i, // 地址输入
    output [31:0] inst   // 指令输出
);
    // 16M
    reg [31:0] mem [0:16777215];
    // 加载
    initial begin
        $readmemh("./resources/inst.hex", mem);
        $display("ROM: Loaded instruction from resources/inst.hex");
    end

    assign inst = mem[rom_i[23:0]];

endmodule