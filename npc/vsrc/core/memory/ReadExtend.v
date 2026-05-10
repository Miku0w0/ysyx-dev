// vsrc/core/memory/ReadExtend.v
module ReadExtend (
    input        is_load,
    input        is_lw, is_lh, is_lb, is_lhu, is_lbu,
    input [1:0]  addr_offset,
    input [31:0] mem_rdata_raw,
    output reg [31:0] mem_rdata_o
);
    always @(*) begin
        mem_rdata_o = 32'hdeadbeef;
        if (!is_load) begin
            mem_rdata_o = 32'h0;
        end
        else case (addr_offset)
            2'b00: begin
                if (is_lb)       mem_rdata_o = {{24{mem_rdata_raw[7]}}, mem_rdata_raw[7:0]};
                else if (is_lbu) mem_rdata_o = {24'b0, mem_rdata_raw[7:0]};
                else if (is_lh)  mem_rdata_o = {{16{mem_rdata_raw[15]}}, mem_rdata_raw[15:0]};
                else if (is_lhu) mem_rdata_o = {16'b0, mem_rdata_raw[15:0]};
                else if (is_lw)  mem_rdata_o = mem_rdata_raw;
            end
            2'b01: begin
                if (is_lb)       mem_rdata_o = {{24{mem_rdata_raw[15]}}, mem_rdata_raw[15:8]};
                else if (is_lbu) mem_rdata_o = {24'b0, mem_rdata_raw[15:8]};
            end
            2'b10: begin
                if (is_lb)       mem_rdata_o = {{24{mem_rdata_raw[23]}}, mem_rdata_raw[23:16]};
                else if (is_lbu) mem_rdata_o = {24'b0, mem_rdata_raw[23:16]};
                else if (is_lh)  mem_rdata_o = {{16{mem_rdata_raw[31]}}, mem_rdata_raw[31:16]};
                else if (is_lhu) mem_rdata_o = {16'b0, mem_rdata_raw[31:16]};
                else if (is_lw)  mem_rdata_o = mem_rdata_raw;
            end
            2'b11: begin
                if (is_lb)       mem_rdata_o = {{24{mem_rdata_raw[31]}}, mem_rdata_raw[31:24]};
                else if (is_lbu) mem_rdata_o = {24'b0, mem_rdata_raw[31:24]};
            end
        endcase
    end
endmodule