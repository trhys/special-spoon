import subprocess
import sys

def get(packages):
    """
    Checks if system packages are installed and installs them if they are not.
    Requires sudo/root privileges to install packages.
    """
    print(f"Checking for required packages: {', '.join(packages)}")
    
    # Determine the package manager
    try:
        subprocess.run(["apt-get", "--version"], capture_output=True, check=True)
        pkg_manager_check = "dpkg-query"
        pkg_manager_install = "apt-get"
        check_args = ["-s"]
    except FileNotFoundError:
        try:
            subprocess.run(["yum", "--version"], capture_output=True, check=True)
            pkg_manager_check = "yum"
            pkg_manager_install = "yum"
            check_args = ["list", "installed"]
        except FileNotFoundError:
            print("ERROR: Unsupported package manager.")
            sys.exit(1)

    missing_packages = []
    for package in packages:
        # Check if the package is installed
        try:
            # dpkg-query -s returns non-zero if package not found
            # yum list installed returns non-zero if package not found
            subprocess.run([pkg_manager_check] + check_args + [package], check=True, capture_output=True)
            print(f"* {package} is already installed.")
        except subprocess.CalledProcessError:
            print(f"* {package} is missing.")
            missing_packages.append(package)

    if missing_packages:
        print("\nThe following packages need to be installed:")
        print(", ".join(missing_packages))
        
        # Attempt to install missing packages
        # This will likely require the script to be run with `sudo`
        install_command = [pkg_manager_install, "install", "-y"] + missing_packages
        print(f"Running command: {' '.join(install_command)}")
        
        try:
            # Note: This will prompt for a password if not run with sudo
            subprocess.run(install_command, check=True, stdin=sys.stdin, stdout=sys.stdout, stderr=sys.stderr)
            print("\nSuccessfully installed missing packages.")
        except subprocess.CalledProcessError as e:
            print(f"\nFailed to install packages. Please run the script with sudo privileges or install manually: {e}")
            sys.exit(1)

    else:
        print("\nAll required packages are installed.")

def buildme():
    # Presets in CMakePresets.json
    presets = subprocess.run(["cmake --list-presets"], capture_output=True)

    print("\nGenerating build directory\n")

    subprocess.run(["mkdir build"])
    subprocess.run(["cd build"])

    selecting = True
    print(f"Select preset: {presets.stdout}")
    while selecting:
        configuration = input("\n: ")
        if configuration == "sandbox-debug":
            preset_out_dir = "debug"
            selecting = False
        elif configuration == "sandbox-release":
            preset_out_dir == "release"
            selecting = False
        elif configuration == "core":
            preset_out_dir = ""
            selecting = False
        else: print("\nInvalid preset\n\n")

    try:
        preset_cmd = "cmake .. --preset="
        subprocess.run([preset_cmd] + [configuration], check=True)
        print("\n Configuration successful \n")
    except subprocess.CalledProcessError as e:
        print(f"\nFailed to configure project:\n\n {e}")
        sys.exit(1)
    
    try:
        print("Finding config directory...\n")
        subprocess.run(["cd"] + [preset_out_dir], check=True)
    except subprocess.CalledProcessError as e:
        print(f"\nCannot find configuration directory:\n\n {e}")
        sys.exit(1)

    print("Starting CMake build\n")
    try:
        subprocess.run(["cmake --build ."], check=True)
    except subprocess.CalledProcessError as e:
        print(f"\nCannot begin CMake build:\n\n {e}")
        sys.exit(1)
    
    
if __name__ == "__main__":

    required_packages = ["libxrandr-dev", "libxcursor-dev", "libxi-dev", "libudev-dev",
        "libopengl-dev", "libgl-dev", "libharfbuzz-dev", "libogg-dev", "libvorbis-dev", "libflac-dev"]
    
    get(required_packages)