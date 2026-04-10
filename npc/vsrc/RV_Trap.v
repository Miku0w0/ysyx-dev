module RV_Trap(
    input        clk,
    input        is_ebreak,
    input [31:0] pc     
);
    import "DPI-C" function void set_ebreak(); // 调用set_ebreak()
    always @(*) begin
        if (is_ebreak) begin
            set_ebreak();
        end
    end
endmodule