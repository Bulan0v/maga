`timescale 1ns/1ps

module tb_axis_fifo_split;

    parameter DEPTH = 4;
    parameter CLK_PERIOD = 10;

    logic clk;
    logic rst;

    logic [63:0] s_axis_tdata;
    logic        s_axis_tvalid;
    // logic        s_axis_tlast;
    logic        s_axis_tready;
    logic [31:0] m_axis_tdata0;
    logic        m_axis_tuser0;
    // logic        m_axis_tlast0;
    logic        m_axis_tvalid0;
    logic        m_axis_tready0;
    logic [31:0] m_axis_tdata1;
    logic        m_axis_tuser1;
    // logic        m_axis_tlast1;
    logic        m_axis_tvalid1;
    logic        m_axis_tready1;

    int num_words = 8;

    logic [31:0] mem0 [DEPTH];
    logic [31:0] mem1 [DEPTH];

    bit user0, user1;

    int wr_cnt = 0;
    int rd_lower_cnt = 0;
    int rd_upper_cnt = 0;

    axis_fifo_split #(
        .DEPTH(DEPTH)
    ) dut (
        .clk(clk),
        .rst(rst),
        .s_axis_tdata(s_axis_tdata),
        .s_axis_tvalid(s_axis_tvalid),
        // .s_axis_tlast(s_axis_tlast),
        .s_axis_tready(s_axis_tready),
        .m_axis_tdata0(m_axis_tdata0),
        .m_axis_tuser0(m_axis_tuser0),
        // .m_axis_tlast0(m_axis_tlast0),
        .m_axis_tvalid0(m_axis_tvalid0),
        .m_axis_tready0(m_axis_tready0),
        .m_axis_tdata1(m_axis_tdata1),
        .m_axis_tuser1(m_axis_tuser1),
        // .m_axis_tlast1(m_axis_tlast1),
        .m_axis_tvalid1(m_axis_tvalid1),
        .m_axis_tready1(m_axis_tready1)
    );

    initial begin
        clk = 0;
        forever #(CLK_PERIOD/2) clk = ~clk;
    end

    initial begin
        forever begin
            wait(wr_cnt > 0);
            user0 = 1;
            user1 = 0;
            wait(rst == 1); 
        end
    end

    initial begin
        fork
            forever begin
                wait(rd_upper_cnt == DEPTH);
                rd_upper_cnt = 0;
            end
            forever begin
                wait(rd_lower_cnt == DEPTH);
                rd_lower_cnt = 0;
            end
        join
    end

    task write_word(input bit [63:0] i_data = {$urandom(), $urandom()});
        @(posedge clk);
        s_axis_tdata = i_data;
        s_axis_tvalid = 1;
        wait (s_axis_tready);
        @(posedge clk);
        s_axis_tvalid = 0;
        mem0[(wr_cnt % DEPTH)] = i_data[31:0];
        mem1[(wr_cnt % DEPTH)] = i_data[63:32];
        wr_cnt++;
    endtask

    task read_cycle(input logic ready0, input logic ready1);
        if(!ready0 && !ready1)
            $error("!ready0 && !ready1");
        m_axis_tready0 = ready0;
        m_axis_tready1 = ready1;
        do begin
            @(posedge clk);
            #0;
        end 
        while (!((ready0 && m_axis_tvalid0) || (ready1 && m_axis_tvalid1) || (!ready0 && !ready1)));
        fork
            if (ready0 && m_axis_tvalid0) begin
                assert (m_axis_tuser0 == user0) else
                    $error("Time %0d: m_axis_tuser0 mismatch: expected %b, got %b", $time, user0, m_axis_tuser0);
                assert (m_axis_tdata0 == mem0[rd_lower_cnt]) else
                    $error("Time %0d: m_axis_tdata0 mismatch at index %0d: expected %h, got %h", $time, rd_lower_cnt, mem0[rd_lower_cnt], m_axis_tdata0);
                rd_lower_cnt++;
            end
            if (ready1 && m_axis_tvalid1) begin
                assert (m_axis_tuser1 == user1) else
                    $error("Time %0d: m_axis_tuser1 mismatch: expected %b, got %b", $time, user1, m_axis_tuser1);
                assert (m_axis_tdata1 == mem1[rd_upper_cnt]) else
                    $error("Time %0d: m_axis_tdata1 mismatch at index %0d: expected %h, got %h", $time, rd_upper_cnt, mem1[rd_upper_cnt], m_axis_tdata1);
                rd_upper_cnt++;
            end
        join

        if(ready0 && m_axis_tvalid0 && !ready1) begin
            user0 = 0;
            user1 = 1;
        end
        else if(ready1 && m_axis_tvalid1 && !ready0) begin
            user0 = 1;
            user1 = 0;
        end

        m_axis_tready0 = 0;
        m_axis_tready1 = 0;
    endtask

    initial begin
        reset();

        for (int i = 0; i < (DEPTH-1); i++) write_word();
        repeat (2) @(posedge clk);
        @(negedge clk);
        for (int i = 0; i < (DEPTH-1); i++) read_cycle(1, 1);
        @(posedge clk);

        for (int i = 0; i < DEPTH; i++) write_word();
        repeat (2) @(posedge clk);
        @(negedge clk);
        for (int i = 0; i < DEPTH; i++) read_cycle(1, 1);

        @(posedge clk);
        
        write_word();
        write_word();
        write_word();
        write_word();
        repeat (2) @(posedge clk);
        @(negedge clk);
        read_cycle(1, 0);
        @(negedge clk);
        read_cycle(0, 1);
        @(negedge clk);
        read_cycle(1, 1);

        reset();
        
        for (int i = 0; i < DEPTH; i++) begin
            write_word();
        end
        fork
            begin 
                write_word(); 
            end
            begin
                repeat (DEPTH) begin
                    @(negedge clk);
                    read_cycle(1,1);
                end
            end
        join
        @(posedge clk);

        $finish;
    end

    task automatic reset();
        init();
        rst = 1;
        repeat (3) @(posedge clk);
        rst = 0;
    endtask : reset

    task automatic init();
        s_axis_tvalid = 0;
        m_axis_tready0 = 0;
        m_axis_tready1 = 0;
        wr_cnt = 0;
        rd_lower_cnt = 0;
        rd_upper_cnt = 0;
    endtask : init

endmodule