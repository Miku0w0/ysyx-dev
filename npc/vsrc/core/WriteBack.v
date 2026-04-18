module WriteBack(
    input  [31:0] alu_res,       
    input  [31:0] mem_rdata_out,
    input  [31:0] snpc,          
    input  [31:0] u_imm,         
    input         is_load,               
    input         is_lui,        
    input         is_jalr,       

    output [31:0] wdata_i        // 最终写回寄存器的
);

    assign wdata_i =
        is_load ? mem_rdata_out :  // Load
        is_jalr ? snpc          :  // jal/jalr
        is_lui  ? u_imm         :  // lui
                  alu_res;         // 其他 ALU 指令
endmodule