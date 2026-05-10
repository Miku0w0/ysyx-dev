// vsrc/core/execute/BranchJudge.v
module BranchJudge (
    input [31:0] rdata1, rdata2,
    input [2:0] funct3,
    input is_branch,
    output reg take_branch
);
    always @(*) begin
        if (!is_branch) take_branch = 1'b0;
        else case (funct3)
            3'b000: take_branch = (rdata1 == rdata2);
            3'b001: take_branch = (rdata1 != rdata2);
            3'b100: take_branch = ($signed(rdata1) < $signed(rdata2));
            3'b101: take_branch = ($signed(rdata1) >= $signed(rdata2));
            3'b110: take_branch = (rdata1 < rdata2);
            3'b111: take_branch = (rdata1 >= rdata2);
            default: take_branch = 1'b0;
        endcase
    end
endmodule