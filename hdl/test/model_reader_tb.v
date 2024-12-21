module tb_model_reader();

    reg clk;
    reg rst_n;
    reg start_read;
    wire read_done;

    //Instantiate the model_reader
    model_reader #(
        .MAX_LAYERS(10),
        .MAX_NEURONS(1024),
        .WEIGHT_WIDTH(8),
        .ADDR_WIDTH(16)
    ) uut (
        .clk(clk),
        .rst_n(rst_n),
        .start_read(start_read),
        .read_done(read_done)
    );

    //Clock generation
    always #5 clk = ~clk;

    initial begin
        //Initialize signals
        clk        = 0;
        rst_n      = 0;
        start_read = 0;

        //Reset
        #20 rst_n = 1;

        //Start reading
        #10 start_read = 1;
        #10 start_read = 0;

        //Wait for read_done
        wait(read_done);
        
        #100 $finish;
    end

    //Monitor read_done signal
    always @(posedge read_done) begin
        $display("Model reading completed");
    end

endmodule