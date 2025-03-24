import os
import shutil

def before_fs_upload(source, target, env):
    # Path to the node_modules folder inside the LittleFS data directory
    node_modules_path = os.path.join(env.subst("$PROJECT_DIR"), "data/www/node_modules")

    # Check if the node_modules folder exists
    if os.path.exists(node_modules_path):
        print(f"Removing {node_modules_path} before uploading to LittleFS...")
        # Delete the node_modules folder and its contents
        shutil.rmtree(node_modules_path)

# Hook the function into the build process
Import("env")
env.AddPreAction("$BUILD_DIR/littlefs.bin", before_fs_upload)
