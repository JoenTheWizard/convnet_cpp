module neuron #(
    parameter SIZE  = 2,  //Number of elements in each array
    parameter WIDTH = 8   //Bit width of each element
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

localparam IDLE = 2'b00, MULTIPLY = 2'b01, FINISH = 2'b10;

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
                    state <= FINISH;
                end
            end

            FINISH: begin
                result <= accumulator;
                valid  <= 1;
                done   <= 1;
                state  <= IDLE;
            end

            default: state <= IDLE;
        endcase
    end
end

endmodule
