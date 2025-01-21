import numpy as np
import argparse

#Generates the sigmoid LUT values with each line representing their respective signed-value address
#This can be improved but will suffice for now

def sigmoid(x):
    """Calculate the sigmoid function."""
    return 1 / (1 + np.exp(-x))

#Will generate for 8-bit signed values
def generate_lut(num_entries, min_input, max_input, output_file):
    """
    Generate a lookup table for the sigmoid function (8-bit signed values).

    :param num_entries: The number of entries within the Look-up table
    :param min_input: Minimum input value for the LUT
    :param max_input: Maximum input value for the LUT
    :param output_file: Output file name
    """
    entry_halves = num_entries // 2

    first_inputs  = np.round(sigmoid(np.linspace(0, max_input, entry_halves)) * 127).astype(np.int8)
    second_inputs = np.round(sigmoid(np.linspace(min_input, 0, entry_halves)) * 127).astype(np.int8)

    final = np.concatenate((first_inputs, second_inputs))
    print(final)

    with open(output_file, 'w') as f:
        for val in final:
            #print(val, f"{int(val):02X}")
            f.write(f"{int(val):02X}\n")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate a LUT for the sigmoid activation function.")
    parser.add_argument('--num_entries', type=int, default=256, help='Number of entries in the LUT')
    parser.add_argument('--min_input', type=float, default=-6.0, help='Minimum input value for the LUT')
    parser.add_argument('--max_input', type=float, default=6.0, help='Maximum input value for the LUT')
    parser.add_argument('--output_file', type=str, default='sigmoid_lut.mem', help='Output file name')

    args = parser.parse_args()

    generate_lut(args.num_entries, args.min_input, args.max_input, args.output_file)
    print(f"[+] LUT generated and saved to {args.output_file}")
