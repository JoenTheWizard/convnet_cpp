module model_reader #(
    parameter MAX_LAYERS   = 10,
    parameter MAX_NEURONS  = 1024,
    parameter WEIGHT_WIDTH = 8,
    parameter ADDR_WIDTH   = 16
)(
    input wire clk,
    input wire rst_n,
    input wire start_read,
    output reg read_done
);

integer file_handle, bytes_read, offset;
reg [WEIGHT_WIDTH-1:0] data [0:1023];
reg [31:0] num_layers, num_rows, num_cols;
reg [7:0] max_weight, activation;

//This module is just for reading the quantized model's binary file and printing it
//NOTE: The this reads the models quantized to 8-bit precision

always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        read_done <= 0;
    end
    else if (start_read) begin
        file_handle = $fopen("../xor_q2.weights", "rb");
        if (file_handle == 0) begin
            $display("[-] ERROR: Failed to open file");
        end
        else begin
            bytes_read = $fread(data, file_handle);

            //Header
            num_layers = {data[3], data[2], data[1], data[0]};
            max_weight = {data[4]};
            $display("Number of layers: %0d", num_layers);
            $display("Number of signed bits integer occupies: %0d", max_weight);

            //Byte offset from header
            offset = 5;

            $display("======== WEIGHTS =========");

            for (integer i = 0; i < num_layers; i++) begin
                num_rows = {data[offset+3], data[offset+2], data[offset+1], data[offset]};
                offset = offset + 4;
                num_cols = {data[offset+3], data[offset+2], data[offset+1], data[offset]};
                offset = offset + 4;

                $display("Layer %0d:", i+1);
                $display("Number of rows: %0d", num_rows);
                $display("Number of columns: %0d", num_cols);

                for (integer j = 0; j < num_rows; j++) begin
                    for (integer k = 0; k < num_cols; k++) begin
                        $write("%0d ", $signed(data[offset]));
                        offset = offset + 1;
                    end
                    $display("");
                end

                activation = data[offset];
                $display("Activation function: %0d", activation);
                offset = offset + 1;
            end
            
            $fclose(file_handle);
            read_done <= 1;
        end
    end
end

endmodule