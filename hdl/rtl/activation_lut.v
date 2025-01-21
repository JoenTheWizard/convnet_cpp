module activation_lut #(
    //Default will be set to 8-bit sigmoid output generated from gen_activation_lut.py
    parameter LUT_FILE   = "build/sigmoid_lut.mem", 
    parameter ADDR_WIDTH = 8,
    parameter DATA_WIDTH = 8
)(
    input wire clk,
    input wire [ADDR_WIDTH-1:0] address,

    output reg [DATA_WIDTH-1:0] data_out
);

//This module is used to pass the sum of the multiplications (aka address) and return the activation function result

reg [DATA_WIDTH-1:0] activation_rom [0:(2**ADDR_WIDTH)-1];

initial begin
    $readmemh(LUT_FILE, activation_rom);
end

always @(posedge clk) begin
    data_out <= activation_rom[address];
end

endmodule
