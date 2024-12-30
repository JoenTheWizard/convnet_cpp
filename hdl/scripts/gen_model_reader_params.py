import os
import sys

### This is used to generate the Verilog header file to set the parameters during compile time ###

def get_file_size(file_path):
    """
    Get the exact size of a file in bytes.
    
    :param file_path: Path to the file
    :return: Size of the file in bytes
    """
    try:
        file_size = os.path.getsize(file_path)
        return file_size
    except FileNotFoundError:
        print(f"[-] ERROR: File '{file_path}' not found.")
        return None
    except Exception as e:
        print(f"[-] ERROR: An error occurred: {e}")
        return None

def generate_verilog_header(model_path, model_size):
    """
    Generate a Verilog header file with parameters based on the file size.
    
    :param model_path: Path to the input model file
    :param model_size: Size of the model file in bytes
    """
    header_content = f"""`ifndef MODEL_PARAMS_H
`define MODEL_PARAMS_H

`define MODEL_SIZE {model_size}
`define MODEL_PATH "{os.path.abspath(model_path)}"

`endif // MODEL_PARAMS_H"""
    
    header_file_path = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                        "..", "build", "model_params.vh")
    os.makedirs(os.path.dirname(header_file_path), exist_ok=True)

    with open(header_file_path, "w") as header_file:
        header_file.write(header_content)
    
    print(f"[+] Verilog header file '{header_file_path}' generated successfully.")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print(f"[*] Usage: python {sys.argv[0]} <model_path>")
        sys.exit(1)

    file_path = sys.argv[1]
    size = get_file_size(file_path)

    if size is not None:
        print(f"[+] The size of '{file_path}' is {size} bytes.")
        generate_verilog_header(file_path, size)
    else:
        sys.exit(1)