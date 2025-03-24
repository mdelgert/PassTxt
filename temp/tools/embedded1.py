# Not a good idea to modify platformio.ini, better to do it manually

import os

# Directories and files
embedded_dir = "web"
config_file = "embedded_files.txt"
platformio_file = "platformio.ini"
header_file = "include/EmbeddedFiles.h"
snippet_file = "include/EmbeddedFileSnippet.h"

# Files and folders to exclude
excluded_files = ["package-lock.json", "package.json"]
excluded_dirs = ["node_modules", "dist"]

# MIME types for file extensions
mime_types = {
    ".html": "text/html",
    ".css": "text/css",
    ".js": "application/javascript",
}

# Helper to get MIME type
def get_mime_type(filename):
    ext = os.path.splitext(filename)[1]
    return mime_types.get(ext, "application/octet-stream")

# Step 1: Generate embedded files list
embedded_files = []
for root, dirs, files in os.walk(embedded_dir):
    dirs[:] = [d for d in dirs if d not in excluded_dirs]  # Exclude directories
    for file in files:
        if file in excluded_files:
            continue
        embedded_files.append(os.path.join(embedded_dir, file).replace("\\", "/"))

# Write embedded_files.txt
with open(config_file, "w") as f:
    f.write("\n".join(sorted(embedded_files)))

# Step 2: Generate declarations and serveEmbeddedFile calls
declarations = []
serve_calls = []
for file in embedded_files:
    variable_name = file.split("/")[-1].replace(".", "_")  # Extract filename and replace dots with underscores
    mime_type = get_mime_type(file)
    declarations.append(
        f"extern const uint8_t {variable_name}_start[] asm(\"_binary_{file.replace('.', '_')}_start\");"
    )
    declarations.append(
        f"extern const uint8_t {variable_name}_end[] asm(\"_binary_{file.replace('.', '_')}_end\");"
    )
    serve_calls.append(
        f"    serveEmbeddedFile(\"/{os.path.basename(file)}\", {variable_name}_start, {variable_name}_end, \"{mime_type}\");"
    )

# Write EmbeddedFiles.h
with open(header_file, "w") as f:
    f.write("#ifndef EMBEDDED_FILES_H\n#define EMBEDDED_FILES_H\n\n#include <Arduino.h>\n\n")
    f.write("// Declarations for embedded files\n")
    f.write("\n".join(declarations))
    f.write("\n\n#endif // EMBEDDED_FILES_H\n")

# Write EmbeddedFileSnippet.h
with open(snippet_file, "w") as f:
    f.write("#ifndef EMBEDDED_FILE_SNIPPET_H\n#define EMBEDDED_FILE_SNIPPET_H\n\n")
    f.write("// Automatically register embedded files\n")
    f.write("inline void registerEmbeddedFiles() {\n")
    f.write("\n".join(serve_calls))
    f.write("\n}\n\n#endif // EMBEDDED_FILE_SNIPPET_H\n")

# Step 3: Update platformio.ini
if os.path.exists(platformio_file):
    with open(platformio_file, "r") as f:
        lines = f.readlines()

    updated_lines = []
    inside_embed_files = False
    for line in lines:
        if line.startswith("board_build.embed_files"):
            inside_embed_files = True
            updated_lines.append("board_build.embed_files =\n")
            updated_lines.extend(f"    {file}\n" for file in sorted(embedded_files))
        elif inside_embed_files and line.startswith("    "):
            continue  # Skip existing embed file entries
        else:
            inside_embed_files = False
            updated_lines.append(line)

    with open(platformio_file, "w") as f:
        f.writelines(updated_lines)
else:
    with open(platformio_file, "w") as f:
        f.write("[env]\n")
        f.write("board_build.embed_files =\n")
        f.writelines(f"    {file}\n" for file in sorted(embedded_files))

print(f"Updated {platformio_file}, generated {header_file} and {snippet_file}, and wrote {config_file}")
