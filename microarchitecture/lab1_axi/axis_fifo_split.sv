module axis_fifo_split #(
    parameter DEPTH = 16
)(
    input  logic        clk,
    input  logic        rst,

    input  logic [63:0] s_axis_tdata,
    input  logic        s_axis_tvalid,
    // input  logic        s_axis_tlast,
    output logic        s_axis_tready,

    output logic [31:0] m_axis_tdata0,
    output logic        m_axis_tuser0,
    // output logic        m_axis_tlast0,
    output logic        m_axis_tvalid0,
    input  logic        m_axis_tready0,

    output logic [31:0] m_axis_tdata1,
    output logic        m_axis_tuser1,
    // output logic        m_axis_tlast1,
    output logic        m_axis_tvalid1,
    input  logic        m_axis_tready1
);

    localparam ADDR_WIDTH = $clog2(DEPTH);
    localparam CNT_WIDTH  = ADDR_WIDTH + 1;

    bit wr_op, rd0_op, rd1_op;

    bit [CNT_WIDTH-1:0] word0_count;
    bit [CNT_WIDTH-1:0] word1_count;

    logic [31:0] mem0 [0:DEPTH-1];
    logic [31:0] mem1 [0:DEPTH-1];

    logic [31:0] data0_reg, data1_reg;
    logic        user0_reg, user1_reg;
    logic        valid0_reg, valid1_reg;
    // logic        last0_reg, last1_reg;

    assign m_axis_tdata0  = data0_reg;
    assign m_axis_tdata1  = data1_reg;
    assign m_axis_tuser0  = user0_reg;
    assign m_axis_tuser1  = user1_reg;
    // assign m_axis_tlast0  = last0_reg;
    // assign m_axis_tlast1  = last1_reg;
    assign m_axis_tvalid0 = valid0_reg;
    assign m_axis_tvalid1 = valid1_reg;

    bit [ADDR_WIDTH-1:0] wr_addr;
    bit [ADDR_WIDTH-1:0] rd_addr0;
    bit [ADDR_WIDTH-1:0] rd_addr1;

    assign wr_op  = s_axis_tvalid && s_axis_tready;
    assign rd0_op = valid0_reg && m_axis_tready0;
    assign rd1_op = valid1_reg && m_axis_tready1;

    // always_comb begin
    //     if(!valid0_reg || rst) begin
    //         last0_reg = 0;
    //     end
    //     else begin
    //         if(word0_count == 0) last0_reg <= 1;
    //         else                 last0_reg <= 0;
    //     end
    // end

    // always_comb begin
    //     if(!valid1_reg || rst) begin
    //         last1_reg = 0;
    //     end
    //     else begin
    //         if(word1_count == 0) last1_reg <= 1;
    //         else                 last1_reg <= 0;
    //     end
    // end

    always_ff @(posedge clk) begin
        if(rst) begin
            s_axis_tready <= 0;
        end else begin
            s_axis_tready <= (word1_count < (DEPTH-1));
        end
    end

    always_ff @(posedge clk) begin
        logic lower_avail, upper_avail;

        if (rst) begin
            word0_count <= 0;
            word1_count <= 0;
            data0_reg   <= 0;
            data1_reg   <= 0;
            valid0_reg  <= 0;
            valid1_reg  <= 0;
        end else begin
            // Write operation
            if (wr_op) begin
                mem0[wr_addr] <= s_axis_tdata[31:0];
                mem1[wr_addr] <= s_axis_tdata[63:32];
                word0_count   <= word0_count + 1;
                word1_count   <= word1_count + 1;
                wr_addr       <= (wr_addr + 1) % DEPTH;
            end

            if(!valid0_reg || rd0_op) begin
                if(word0_count > 0) begin
                    data0_reg   <= mem0[rd_addr0];
                    valid0_reg  <= 1;
                    word0_count <= word0_count - 1;
                    rd_addr0    <= (rd_addr0 + 1) % DEPTH;
                end
                else begin
                    valid0_reg <= 0;
                end
            end

            if(!valid1_reg || rd1_op) begin
                if(word1_count > 0) begin
                    data1_reg   <= mem1[rd_addr1];
                    valid1_reg  <= 1;
                    word1_count <= word1_count - 1;
                    rd_addr1    <= (rd_addr1 + 1) % DEPTH;
                end
                else begin
                    valid1_reg <= 0;
                end
            end
        end
    end

    always_ff @(posedge clk) begin
        if (rst) begin
            user0_reg   <= 0;
            user1_reg   <= 0;
        end else begin
            if((user0_reg == 0) && (user1_reg == 0)) begin
                if (wr_op) begin
                    user0_reg <= 1;
                    user1_reg <= 0;
                end
                else begin
                    user0_reg <= user0_reg;
                    user1_reg <= user1_reg;
                end
            end
            else begin
                if (rd0_op && !rd1_op) begin 
                    user0_reg <= 0;
                    user1_reg <= 1;
                end
                else if (rd1_op) begin
                    user0_reg <= 1;
                    user1_reg <= 0;
                end
            end
        end
    end

endmodule