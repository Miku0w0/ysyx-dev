module EXU (
    /* from IFU */
    input  [31:0] pc_i,        
    /* from rf */
    input  [31:0] rdata1,
    input  [31:0] rdata2,
    /* from IDU */
    input  [31:0] imm32,
    input  [3:0]  alu_op,
    input         alu_src,
    input  [2:0]  funct3,
    input         is_auipc,
    input         is_branch,
    input         is_jal,
    
    /* to IFU */
    output [31:0] jalr_target,
    output [31:0] jal_target,
    output [31:0] branch_target,
    output        take_branch,
    /* to LSU || to WBU */
    output [31:0] alu_res
);
    /* 操作数选择 */
    wire [31:0] src1 = (is_auipc) ? pc_i : rdata1;
    wire [31:0] src2 = alu_src ? imm32 : rdata2; // I imm32 R rdata2

    /* alu计算 */
    reg [31:0] alu_out;
    always @(*) begin
        case (alu_op)
            4'b0000: alu_out = src1 + src2; // add/addi/load/store
            4'b1000: alu_out = src1 - src2; // sub
            4'b0001: alu_out = src1 << src2[4:0]; // sll(shift logic)
            4'b0100: alu_out = src1 ^ src2; // xor         
            4'b0110: alu_out = src1 | src2; // or    
            4'b0111: alu_out = src1 & src2; // and     
            4'b0010: alu_out = ($signed(src1) < $signed(src2)) ? 32'd1 : 32'd0; // slt signed less
            4'b0011: alu_out = (src1 < src2) ? 32'd1 : 32'd0; //sltu
            4'b0101: alu_out = src1 >> src2[4:0]; // srl     
            4'b1101: alu_out = $signed(src1) >>> src2[4:0]; // SRA
            default: alu_out = src1 + src2; // 加法
        endcase
    end
    assign alu_res = alu_out;

    /* branch细分 */
    reg branch_out;
    always @(*) begin
        if (!is_branch) begin
            branch_out = 1'b0;
        end 
        else begin
            case (funct3)
                3'b000: branch_out = (rdata1 == rdata2);
                3'b001: branch_out = (rdata1 != rdata2);
                3'b100: branch_out = ($signed(rdata1) < $signed(rdata2));
                3'b101: branch_out = ($signed(rdata1) >= $signed(rdata2));
                3'b110: branch_out = (rdata1 < rdata2);
                3'b111: branch_out = (rdata1 >= rdata2);
                default: branch_out = 1'b0;
            endcase
        end
    end
    assign take_branch = branch_out;

    /* 计算跳转地址 */
    assign jalr_target = (rdata1 + imm32) & 32'hfffffffe;
    assign jal_target = pc_i + imm32;
    assign branch_target = pc_i + imm32;
endmodule