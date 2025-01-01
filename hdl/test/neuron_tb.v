module neuron_tb();

    //Inputs
    reg clk;
    reg reset;
    reg [7:0] i1, i2;
    reg [7:0] w1, w2;

    //Outputs
    wire [16:0] result;

    //Neuron module
    neuron uut(
        .clk(clk),
        .reset(reset),
        .i1(i1),
        .i2(i2),
        .w1(w1),
        .w2(w2),
        .result(result)
    );

    //Clock generation
    initial begin
        clk = 0;
        forever #5 clk = ~clk;
    end

    //Test stimulus
    initial begin
        //Initialize inputs
        reset = 1;
        i1 = 0; i2 = 0;
        w1 = 0; w2 = 0;

        //Wait for global reset
        #100;
        reset = 0;

        //Test case 1: Positive inputs and weights
        #10 i1 = 8'd5; i2 = 8'd3; w1 = 8'd2; w2 = 8'd4;
        #30 $display("Test Case 1: i1=%d, i2=%d, w1=%d, w2=%d, result=%d", $signed(i1), $signed(i2), $signed(w1), $signed(w2), $signed(result));
        
        //Test case 2: Negative inputs, positive weights
        #20 i1 = -8'd2; i2 = -8'd3; w1 = 8'd3; w2 = 8'd2;
        #30 $display("Test Case 2: i1=%d, i2=%d, w1=%d, w2=%d, result=%d", $signed(i1), $signed(i2), $signed(w1), $signed(w2), $signed(result));
        
        //Test case 3: Positive inputs, negative weights
        #20 i1 = 8'd4; i2 = 8'd6; w1 = -8'd2; w2 = -8'd3;
        #30 $display("Test Case 3: i1=%d, i2=%d, w1=%d, w2=%d, result=%d", $signed(i1), $signed(i2), $signed(w1), $signed(w2), $signed(result));
        
        //Test case 4: All negative
        #20 i1 = -8'd3; i2 = -8'd2; w1 = -8'd4; w2 = -8'd5;
        #30 $display("Test Case 4: i1=%d, i2=%d, w1=%d, w2=%d, result=%d", $signed(i1), $signed(i2), $signed(w1), $signed(w2), $signed(result));

        //Finish simulation
        #20 $finish;
    end

endmodule