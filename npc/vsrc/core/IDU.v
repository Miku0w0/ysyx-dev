// vsrc/core/decode/IDU.v
module IDU (
    input  [31:0] inst,
    
    /* 字段提取 */
    output [2:0]  funct3,
    output [4:0]  waddr, rs1, rs2,
    /* 指令类型识别 */
    output        is_load, is_store,
    output        is_lui, is_auipc, is_jal, is_jalr, is_branch,
    /* 立即数生成 */
    output [31:0] u_imm, imm32,
    /* 控制信号生成 */
    output [3:0]  alu_op,
    output        wen, alu_src,
    /* 访存细分 */
    output        is_lw, is_lb, is_lbu, is_lh, is_lhu,
    output        is_sw, is_sb, is_sh
);

    /* 字段提取 */
    wire [6:0] opcode;
    InstField u_instfield (
        .inst(inst),

        .rs1(rs1),
        .rs2(rs2),
        .rd(waddr),
        .funct3(funct3),
        .opcode(opcode)
    );
    
    /* 指令类型识别 */
    wire is_r_type, is_i_type;
    Decoder u_decoder (
        .opcode(opcode),

        .is_lui(is_lui),
        .is_auipc(is_auipc),
        .is_jal(is_jal),
        .is_jalr(is_jalr),
        .is_branch(is_branch),
        .is_load(is_load),
        .is_store(is_store),
        .is_r_type(is_r_type),
        .is_i_type(is_i_type)
    );
    
    /* 立即数生成 */
    wire is_lui_auipc = is_lui | is_auipc;
    ImmGen u_immgen (
        .inst(inst),
        .is_store(is_store),
        .is_branch(is_branch),
        .is_lui_auipc(is_lui_auipc),
        .is_jal(is_jal),

        .imm32(imm32),
        .u_imm(u_imm)
    );
    
    /* 控制信号生成 */
    CtrlGen u_ctrlgen (
        .inst(inst),
        .is_r_type(is_r_type),
        .is_i_type(is_i_type),
        .is_load(is_load),
        .is_store(is_store),
        .is_auipc(is_auipc),
        .is_branch(is_branch),
        .funct3(funct3),

        .alu_src(alu_src),
        .wen(wen),
        .alu_op(alu_op)
    );
    
    /* 访存细分 */
    LSDecoder u_lsdecoder (
        .is_load(is_load),
        .is_store(is_store),
        .funct3(funct3),

        .is_lw(is_lw),
        .is_lb(is_lb),
        .is_lbu(is_lbu),
        .is_lh(is_lh),
        .is_lhu(is_lhu),
        .is_sw(is_sw),
        .is_sb(is_sb),
        .is_sh(is_sh)
    );

endmodule