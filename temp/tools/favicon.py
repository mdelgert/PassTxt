input_file = "../web/favicon.ico"
output_file = "favicon_ico.h"

with open(input_file, "rb") as f:
    data = f.read()

with open(output_file, "w") as f:
    f.write("unsigned char favicon_ico[] = {\n")
    for i, byte in enumerate(data):
        f.write(f"  0x{byte:02x},")
        if (i + 1) % 12 == 0:  # 12 bytes per line
            f.write("\n")
    f.write("\n};\n")
    f.write(f"unsigned int favicon_ico_len = {len(data)};\n")
