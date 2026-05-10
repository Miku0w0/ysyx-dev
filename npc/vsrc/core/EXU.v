module EXU (
    input [31:0] pc_i, rdata1, rdata2, imm32,
    input [3:0] alu_op,
    input alu_src,
    input [2:0] funct3,
    input is_auipc, is_branch,
    output [31:0] jalr_target, jal_target, branch_target, alu_res,
    output take_branch
);
    /* 操作数选择 */
    wire [31:0] src1 = (is_auipc) ? pc_i : rdata1;
    wire [31:0] src2 = alu_src ? imm32 : rdata2;
    /* ALU计算 */
    ALU u_alu (
        .src1(src1),
        .src2(src2),
        .alu_op(alu_op),

        .alu_res(alu_res)
    );
    /* 分支判断 */
    BranchJudge u_branchjudge (
        .rdata1(rdata1),
        .rdata2(rdata2),
        .funct3(funct3),
        .is_branch(is_branch),
        
        .take_branch(take_branch)
    );
    /* 跳转目标计算 */
    JumpTarget u_jumptarget (
        .pc_i(pc_i),
        .rdata1(rdata1),
        .imm32(imm32),

        .jalr_target(jalr_target),
        .jal_target(jal_target),
        .branch_target(branch_target)
    );
endmodule