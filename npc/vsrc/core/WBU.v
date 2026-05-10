module WBU (
    /* from EXU */
    input  [31:0] alu_res,       
    /* from LSU */
    input  [31:0] mem_rdata_o, 
    /* from IFU */
    input  [31:0] snpc,          
    /* from IDU */
    input  [31:0] u_imm,         
    input         is_lui,        
    input         is_load,           
    input         is_jal, is_jalr,   

    output reg [31:0] wdata_i // 写回数据
);
    reg [31:0] wdata_out;
    always @(*) begin
        if (is_load)                wdata_i = mem_rdata_o; // 内存读取的数据
        else if (is_jalr || is_jal) wdata_i = snpc;        // 跳转的返回地址
        else if (is_lui)            wdata_i = u_imm;       // 高位立即数
        else                        wdata_i = alu_res;     // 其他计算结果
    end
endmodule