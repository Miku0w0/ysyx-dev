/* verilator lint_off WIDTHEXPAND */
module ROM (
    /* from IF */
    input  [23:0] rom_i, 
    
    output [31:0] inst   
);
    // 16M 2^24
    reg [31:0] mem [0:16777215];
    // 加载
    initial begin
        $readmemh("./resources/inst.hex", mem);
        $display("ROM: Loaded instruction from resources/inst.hex");
    end

    assign inst = mem[rom_i[23:0]]; // 在取指中已换按字寻址

endmodule