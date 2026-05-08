module LSU (
    input         clk,
    /* from EXU */
    input  [31:0] alu_res,      // 作为访存地址
    /* from rf */
    input  [31:0] rdata2,       // 写入RAM的32位数据，用于**写入**逻辑
    /* from IDU */
    input         is_sw, is_sb, is_sh,  
    input         is_lw, is_lh, is_lb, is_lhu, is_lbu,
    /* to WBU  */
    output reg [31:0] mem_rdata_out,// load时从RAM读出的数据 写回寄存器

    output reg [31:0] ram_data_i,   // 写入数据 rdata2
    output reg        ram_we,       // 写使能    
    output reg [7:0]  ram_wmask     // 写掩码
);
    // ===== 地址偏移 =====
    wire [1:0] addr_offset = alu_res[1:0];

    // ===== 写数据 =====
    always @(*) begin
        if (is_sb)
            ram_data_i = {4{rdata2[7:0]}};
        else if (is_sh)
            ram_data_i = {2{rdata2[15:0]}};
        else
            ram_data_i = rdata2;
    end

    // ===== 写掩码 =====
    always @(*) begin
        if (is_sw)
            ram_wmask = 8'b1111;
        else if (is_sh) begin
            case (addr_offset)
                2'b00: ram_wmask = 8'b0011;
                2'b10: ram_wmask = 8'b1100;
                default: ram_wmask = 8'b0000;
            endcase
        end 
        else if (is_sb) begin
            case (addr_offset)
                2'b00: ram_wmask = 8'b0001;
                2'b01: ram_wmask = 8'b0010;
                2'b10: ram_wmask = 8'b0100;
                2'b11: ram_wmask = 8'b1000;
                default: ram_wmask = 8'b0000;
            endcase
        end 
        else 
            ram_wmask = 8'b0000;
    end

    // ===== 内存读写 =====
    // 写使能
    assign ram_we = (is_sw | is_sh | is_sb);
    // 写操作
    always @(posedge clk) begin
        if (ram_we) begin
            pmem_write(alu_res & 32'hfffffffc, ram_data_i, ram_wmask);
        end
    end
    // 读操作
    wire is_load = is_lw | is_lh | is_lb | is_lhu | is_lbu;
    wire [31:0] ram_data_o = is_load ? pmem_read(alu_res & 32'hfffffffc) : 32'h0;
    always @(*) begin
        case (addr_offset)
            2'b00: mem_rdata_out = (is_lb)  ? {{24{ram_data_o[7]}}, ram_data_o[7:0]} :
                                   (is_lbu) ? {24'b0, ram_data_o[7:0]} :
                                   (is_lh)  ? {{16{ram_data_o[15]}}, ram_data_o[15:0]} :
                                   (is_lhu) ? {16'b0, ram_data_o[15:0]} : ram_data_o;

            2'b01: mem_rdata_out = (is_lb)  ? {{24{ram_data_o[15]}}, ram_data_o[15:8]} :
                                   (is_lbu) ? {24'b0, ram_data_o[15:8]} : 32'hdeadbeef;

            2'b10: mem_rdata_out = (is_lb)  ? {{24{ram_data_o[23]}}, ram_data_o[23:16]} :
                                   (is_lbu) ? {24'b0, ram_data_o[23:16]} :
                                   (is_lh)  ? {{16{ram_data_o[31]}}, ram_data_o[31:16]} :
                                   (is_lhu) ? {16'b0, ram_data_o[31:16]} : 32'hdeadbeef;

            2'b11: mem_rdata_out = (is_lb)  ? {{24{ram_data_o[31]}}, ram_data_o[31:24]} :
                                   (is_lbu) ? {24'b0, ram_data_o[31:24]} : 32'hdeadbeef;
        endcase
    end
endmodule