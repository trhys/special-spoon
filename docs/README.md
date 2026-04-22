# special-spoon
<p align= "center">
<img width="500" height="500" alt="openart-6fce20aa-1c64-4da0-90fe-34ec1595aec4" src="https://github.com/user-attachments/assets/6fd33ac6-2b39-4a6d-9411-03c05ad28c5e" />
</p>

Special Spoon is an application framework designed to be a wrapper for SFML, using ImGui as an editor interface. It uses a modular ECS to create a data-oriented architecture to make 2D application development extensible and maintainable. 

## Prerequisites

- CMake
- C++ (Visual Studio recommended for CMake integration)
- Python (recommended)
  
## Quick Start
First, pull Special-Spoon: ```git clone https://github.com/trhys/special-spoon.git```

If you're on Linux, you can run the buildme.py script to get the dependancies, init submodules, configure and build automatically: (may require sudo priv): ```sudo python3 buildme.py --preset sandbox-debug```

For linux users, you're done here. Depending on your distro there's a chance the script won't get all the packages you need, or the wrong versions. I'll be putting together a guide for this issue.

On windows, you'll need to manually init the submodules : ```git submodule init``` => ```git submodule update```

If you use visual studio with cmake integration, my preferred method is to open the local folder and let the VS handle it. To run the application you'll need to select the /bin/Sandbox.exe startup project. (TODO: screenshots)

If you want to run this manually in the terminal: from the root dir:
- ```mkdir build```
- ```cd build```
- ```cmake .. --preset=sandbox-debug```

Reference CMakePresets.json for --preset flag options.

If config completes normally, run the build command: ```cmake --build .```

## Usage
TODO
