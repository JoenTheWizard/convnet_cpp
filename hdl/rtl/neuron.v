module neuron #(
    parameter SIZE     = 2,                       //Number of elements in each array
    parameter WIDTH    = 8,                       //Bit width of each element
    parameter LUT_FILE = "build/sigmoid_lut.mem"  //Set the activation LUT file
)(
    input wire clk,
    input wire reset,
    input wire start,

    //Input and weight data passed to the neuron
    input wire signed [WIDTH*SIZE-1:0] weights,
    input wire signed [WIDTH*SIZE-1:0] inputs,

    output reg signed [2*WIDTH:0] result,
    output reg valid,
    output reg done
);

reg [1:0] state;
reg [$clog2(SIZE):0] counter;
reg signed [2*WIDTH:0] accumulator;
wire [WIDTH-1:0] activation_result;

localparam IDLE = 2'b00, MULTIPLY = 2'b01, ACTIVATE = 2'b10, FINISH = 2'b11;

//Pass the accumulated values (inputs & weights) to the LUT, the result will
//be latched to the activation_result at the FINISH state
activation_lut #(
    .LUT_FILE(LUT_FILE),
    .ADDR_WIDTH(WIDTH),
    .DATA_WIDTH(WIDTH)
) act_lut (
    .clk(clk),
    .address(accumulator[WIDTH-1:0]),
    .data_out(activation_result)
);

//This module is for a singular sequential neuron cell that multiplies SIZE x SIZE set and appends the result through the accumulator

always @(posedge clk or posedge reset) begin
    if (reset) begin
        state       <= IDLE;
        counter     <= 0;
        result      <= 0;
        accumulator <= 0;
        valid       <= 0;
        done        <= 0;
    end else begin
        case (state)
            IDLE: begin
                if (start) begin
                    state       <= MULTIPLY;
                    counter     <= 0;
                    accumulator <= 0;
                    done        <= 0;
                end
            end

            MULTIPLY: begin
                accumulator <= accumulator + 
                                ($signed(weights[WIDTH*(counter+1)-1 -: WIDTH]) * 
                                $signed(inputs[WIDTH*(counter+1)-1 -: WIDTH]));
                counter <= counter + 1;

                if (counter == SIZE - 1) begin
                    state <= ACTIVATE;
                end
            end

            ACTIVATE: begin
                state <= FINISH;
            end

            FINISH: begin
                result <= activation_result;
                valid  <= 1;
                done   <= 1;
                state  <= IDLE;
            end

            default: state <= IDLE;
        endcase
    end
end

endmodule
