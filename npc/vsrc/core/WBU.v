module WBU (
    /* from EXU */
    input  [31:0] alu_res,       
    /* from LSU */
    input  [31:0] mem_rdata_out, // 从内存读取并处理过读取字节数量的数据
    /* from IFU */
    input  [31:0] snpc,          
    /* from IDU */
    input  [31:0] u_imm,         
    input         is_load,           
    input         is_lui,        
    input         is_jalr,    
    input         is_jal, 

    output [31:0] wdata_i        // 最终写回寄存器的数据
);

    assign wdata_i =
                   is_load ? mem_rdata_out :  // Load 从RAM读出的数据
        (is_jalr | is_jal) ? snpc          :  // jal/jalr 写回返回地址
                   is_lui  ? u_imm         :  // lui 写入高20位立即数
                            alu_res;          // 其他 运算结果
endmodule