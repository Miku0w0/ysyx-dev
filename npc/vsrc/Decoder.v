module Decoder(
    input  [31:0] inst,
    output [4:0]  rs1,
    output [4:0]  rd,
    output [31:0] imm,
    output is_jalr,
    output is_ebreak,
    output wen
);
    wire [6:0] opcode = inst[6:0];

    assign rs1 = inst[19:15];
    assign rd  = inst[11:7];
    assign imm = {{20{inst[31]}}, inst[31:20]}; // I-type 立即数符号扩展
    assign is_ebreak = (inst == 32'h00100073);
    assign is_jalr = (opcode == 7'b1100111);
    MuxKey #(2, 7, 1) mux_wen (
        .out(wen),
        .key(opcode),
        .lut({
            7'b0010011, 1'b1, // I-type
            7'b1100111, 1'b1 // jalr
        }) 
    );
endmodule