import sys
import os

def file_to_header(input_file, output_file, var_name, namespace = ""):    
    with open(input_file, 'rb') as f:
        data = f.read()
    
    if namespace != "":
        namespace = "::" + namespace

    with open(output_file, 'w') as f:
        f.write("#pragma once\n")
        f.write("#include <array>\n\n")
        f.write(f"namespace Shaders{namespace} {{\n")
        f.write(f"  inline constexpr std::array<char, {len(data) + 1}> {var_name} = {{\n        ")
        for i, byte in enumerate(data):
            f.write(f"0x{byte:02x}, ")
            if (i + 1) % 12 == 0: f.write("\n        ")
        f.write("0x00\n    };\n")
        f.write("}\n")
    

if __name__ == "__main__":
    if len(sys.argv) == 5:
        file_to_header(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4])
    else:
        file_to_header(sys.argv[1], sys.argv[2], sys.argv[3])