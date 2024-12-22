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

integer file_handle, bytes_read;
reg [WEIGHT_WIDTH-1:0] data [0:1023];
reg [31:0] num_layers, num_rows, num_cols;

//This module is just for reading the binary file and loading it into registers

always @(posedge clk or negedge rst_n) begin
    if (!rst_n) begin
        read_done <= 0;
    end 
    else if (start_read) begin
        file_handle = $fopen("../mnist_quantized.weights", "rb");
        if (file_handle == 0) begin
            $display("[-] ERROR: Failed to open file");
        end 
        else begin
            bytes_read = $fread(data, file_handle);

            num_layers = {data[3], data[2], data[1], data[0]};
            num_rows   = {data[7], data[6], data[5], data[4]};
            num_cols   = {data[11], data[10], data[9], data[8]};
            $display("Number of layers: %0d", num_layers);
            $display("Number of rows: %0d", num_rows);
            $display("Number of columns: %0d", num_cols);
            
            $fclose(file_handle);
            read_done <= 1;
        end
    end
end

endmodule