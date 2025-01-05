import numpy as np
import argparse

### Generate look-up table for activation function. Only has sigmoid so far ###

def sigmoid(x):
    """Calculate the sigmoid function."""
    return 1 / (1 + np.exp(-x))

def generate_lut(num_entries, min_input, max_input, output_file):
    """
    Generate a lookup table for the sigmoid function.

    :param num_entries: The number of entries within the Look-up table
    :param min_input: Minimum input value for the LUT
    :param max_input: Maximum input value for the LUT
    :param output_file: Output file name
    """
    #Create an array of input values ranging from min_input to max_input and calculate the sigmoid values
    inputs = np.linspace(min_input, max_input, num_entries)
    lut_values = sigmoid(inputs)
    
    #Scale to 8-bit signed integer range [-128, 127]
    lut_scaled = np.round(lut_values * 127).astype(np.int8)
    
    #Write LUT to the output file
    with open(output_file, 'w') as f:
        for value in lut_scaled:
            #Write each value in two's complement hex format
            #Make sure to mask to get the lower 8 bits
            f.write(f"{value & 0xFF:02X}\n")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate a LUT for the sigmoid activation function.")
    parser.add_argument('--num_entries', type=int, default=256, help='Number of entries in the LUT')
    parser.add_argument('--min_input', type=float, default=-6.0, help='Minimum input value for the LUT')
    parser.add_argument('--max_input', type=float, default=6.0, help='Maximum input value for the LUT')
    parser.add_argument('--output_file', type=str, default='sigmoid_lut.mem', help='Output file name')

    args = parser.parse_args()

    generate_lut(args.num_entries, args.min_input, args.max_input, args.output_file)
    print(f"LUT generated and saved to {args.output_file}")