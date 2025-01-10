module activation_lut_tb;

    //Parameters
    parameter LUT_FILE   = "sigmoid_lut.mem";
    parameter ADDR_WIDTH = 8;
    parameter DATA_WIDTH = 8;

    //Signals
    reg clk;
    reg  [ADDR_WIDTH-1:0] address;
    wire [DATA_WIDTH-1:0] data_out;

    //Activation Function LUT module
    activation_lut #(
        .LUT_FILE(LUT_FILE),
        .ADDR_WIDTH(ADDR_WIDTH),
        .DATA_WIDTH(DATA_WIDTH)
    ) uut (
        .clk(clk),
        .address(address),
        .data_out(data_out)
    );

    //Clock
    always #5 clk = ~clk;

    initial begin
        $dumpfile("build/activation_lut_tb.vcd");
        $dumpvars(0, activation_lut_tb);

        //Init inputs and wait for global reset
        clk     = 0;
        address = 0;
        #100;
        
        //Read from address 0
        address = 8'd0;
        #10;
        $display("Address: %d, Data: %h", address, data_out);

        //Read from address 127
        address = 8'd127;
        #10;
        $display("Address: %d, Data: %h", address, data_out);

        //Read from address 255
        address = 8'd255;
        #10;
        $display("Address: %d, Data: %h", address, data_out);

        //Read from a few random addresses
        repeat(5) begin
            address = $random % 256;
            #10;
            $display("Address: %d, Data: %h", address, data_out);
        end

        #100;
        $finish;
    end

endmodule