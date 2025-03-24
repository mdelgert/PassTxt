from os.path import join
import subprocess

# Import the PlatformIO environment (env)
Import("env")

def before_build_fs(source, target, env):
    print("Building LittleFS filesystem...")
    result = subprocess.run(
        ["platformio", "run", "--target", "buildfs"],
        capture_output=True,
        text=True
    )
    if result.returncode != 0:
        print("Error building LittleFS filesystem:")
        print(result.stderr)
        env.Exit(1)
    else:
        print("LittleFS filesystem built successfully.")
        print(result.stdout)

# Attach the build action to run before the main firmware build
env.AddPreAction("buildprog", before_build_fs)
