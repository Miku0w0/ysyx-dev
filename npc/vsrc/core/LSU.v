module LSU (
    input         clk,
    /* from EXU */
    input  [31:0] alu_res,          // 用于访存地址
    /* from rf */
    input  [31:0] rdata2,           // 用于写入
    /* from IDU */
    input         is_sw, is_sb, is_sh,  
    input         is_lw, is_lh, is_lb, is_lhu, is_lbu,
    /* to WBU  */
    output            mem_we,       // 写使能    
    output reg [7:0]  mem_wmask,    // 写掩码
    output reg [31:0] mem_rdata_o,   // 读出的数据 
    output reg [31:0] mem_wdata_i    // 写入的数据 
);

    /* 写使能 */
    assign mem_we = (is_sw | is_sh | is_sb);
    /* 准备写入的数据 */
    always @(*) begin
        if (is_sb)      mem_wdata_i = {4{rdata2[7:0]}};
        else if (is_sh) mem_wdata_i = {2{rdata2[15:0]}};
        else            mem_wdata_i = rdata2;
    end
    /* 地址偏移量 */
    wire [1:0] addr_offset = alu_res[1:0];
    /* 写掩码 */
    always @(*) begin
        if (is_sw)       mem_wmask = 8'b1111;
        else if (is_sh) begin
            case (addr_offset)
                2'b00:   mem_wmask = 8'b0011;
                2'b10:   mem_wmask = 8'b1100;
                default: mem_wmask = 8'b0000;
            endcase
        end 
        else if (is_sb) begin
            case (addr_offset)
                2'b00:   mem_wmask = 8'b0001;
                2'b01:   mem_wmask = 8'b0010;
                2'b10:   mem_wmask = 8'b0100;
                2'b11:   mem_wmask = 8'b1000;
                default: mem_wmask = 8'b0000;
            endcase
        end 
        else             mem_wmask = 8'b0000;
    end
    /* ===== 内存写 ===== */
    always @(posedge clk) begin
        if (mem_we) begin
            pmem_write({alu_res[31:2], 2'b0}, mem_wdata_i, mem_wmask);
        end
    end

    /* 读使能 */
    wire is_load = is_lw | is_lh | is_lb | is_lhu | is_lbu;
    /* 读出的未处理的原始数据 */
    reg [31:0] mem_rdata_raw;
    always @(*) begin
        if (is_load) mem_rdata_raw = pmem_read({alu_res[31:2], 2'b0});
        else         mem_rdata_raw = 32'h0;
    end
    /* ===== 内存读 ===== */
    always @(*) begin
        mem_rdata_o = 32'hdeadbeef;
        if (!is_load)            mem_rdata_o = 32'h0;
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
                // lh/lhu/lw在非对齐地址需要异常处理
            end
            2'b10: begin
                if (is_lb)       mem_rdata_o = {{24{mem_rdata_raw[23]}}, mem_rdata_raw[23:16]};
                else if (is_lbu) mem_rdata_o = {24'b0, mem_rdata_raw[23:16]};
                else if (is_lh)  mem_rdata_o = {{16{mem_rdata_raw[31]}}, mem_rdata_raw[31:16]};
                else if (is_lhu) mem_rdata_o = {16'b0, mem_rdata_raw[31:16]};
                else if (is_lw)  mem_rdata_o = mem_rdata_raw;
            end
            2'b11: begin
                if (is_lb)      mem_rdata_o = {{24{mem_rdata_raw[31]}}, mem_rdata_raw[31:24]};
                else if (is_lbu) mem_rdata_o = {24'b0, mem_rdata_raw[31:24]};
                // lh/lhu/lw在非对齐地址需要异常处理
            end
        endcase
    end
endmodule