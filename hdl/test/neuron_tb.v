module neuron_tb();

    //Set parameters
    parameter WIDTH = 8;
    parameter SIZE  = 2;

    //Set signals
    reg clk;
    reg reset;
    reg start;
    reg signed [WIDTH*SIZE-1:0] weights;
    reg signed [WIDTH*SIZE-1:0] inputs;
    wire signed [2*WIDTH:0] result;
    wire valid;
    wire done;

    //Neuron module
    neuron #(.SIZE(SIZE), .WIDTH(WIDTH)) uut (
        .clk(clk),
        .reset(reset),
        .start(start),
        .weights(weights),
        .inputs(inputs),
        .result(result),
        .valid(valid),
        .done(done)
    );

    //Clock
    always #5 clk = ~clk;

    integer test_case;

    initial begin
        $dumpfile("neuron_tb.vcd");
        $dumpvars(0, neuron_tb);
        
        //Init inputs
        clk       = 0;
        reset     = 1;
        start     = 0;
        test_case = 0;

        test_case = 1;
        run_test(4'd4, 4'd3, 4'd2, 4'd2);

        test_case = 2;
        run_test(-4'd2, -4'd1, 4'd3, 4'd4);

        test_case = 3;
        run_test(4'd2, 4'd3, -4'd1, -4'd2);

        test_case = 4;
        run_test(-4'd3, 4'd2, 4'd2, -4'd1);

        test_case = 5;
        run_test(4'd0, 4'd5, 4'd3, 4'd2);

        $display("All tests completed");
        $finish;
    end

    //Run test cases
    task run_test;
        input signed [WIDTH-1:0] w1, w0, i1, i0;
        begin
            $display("Running Test Case %0d", test_case);
            weights = {w1, w0};
            inputs  = {i1, i0};

            //NOTE: Reset here might be unnecessary in the test cases
            reset = 1;
            #10 reset = 0;
            #10 start = 1;
            #10 start = 0;

            wait(done);

            //Wait a couple cycles
            #15;
        end
    endtask

    //Display results
    always @(posedge clk) begin
        if (done && valid) begin
            $display("Test Case %0d - Time %0t: Result = %0d", test_case, $time, $signed(result));
        end
    end

endmodule