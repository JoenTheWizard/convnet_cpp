module neuron(
    input wire clk,
    input wire reset,

    input [7:0] i1, i2,
    input [7:0] w1, w2,

    output reg [16:0] result
);

//Right now this is a test with a basic neuron functionality for a 2x2 input-weight

reg [15:0] product1, product2;
reg [16:0] sum;

always @(posedge clk or posedge reset) begin
    if (reset) begin
        product1 <= 0;
        product2 <= 0;
        sum      <= 0;
        result   <= 0;
    end 
    else begin
        //Multiply inputs with weights
        product1 <= $signed(i1) * $signed(w1);
        product2 <= $signed(i2) * $signed(w2);

        //Sum the products
        sum <= $signed(product1) + $signed(product2);

        //Assign sum to result
        result <= sum;
    end
end

endmodule