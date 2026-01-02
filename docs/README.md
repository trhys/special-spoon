# special-spoon
<p align= "center">
<img width="250" height="400" alt="openart-6fce20aa-1c64-4da0-90fe-34ec1595aec4" src="https://github.com/user-attachments/assets/6fd33ac6-2b39-4a6d-9411-03c05ad28c5e" />
</p>

Special-spoon is an application framework using SFML and ImGui. The editor currently uses the Nlohmann json library for serialization. You can create, save, and load scenes built using an ECS design, with the ability to integrate your own custom components and systems.

This allows Special-Spoon to create not only games but other windowed applications using SFML's 2D rendering framework.

# Build Guide
First, clone Special-Spoon onto your local machine. ```git clone https://github.com/trhys/special-spoon.git```

If you're on Linux, you can run the buildme.py script to get the dependancies, init submodules, configure and build automatically: (may require sudo priv) ```sudo python3 buildme.py --preset sandbox-debug```
For linux users, you're done here. Depending on your distro there's a chance the script won't get all the packages you need, or the wrong versions. I'll be putting together a guide for this issue.

On windows, you'll need to manually init the submodules : ```git submodule init``` // ```git submodule update```

If you use visual studio with cmake integration, my preferred method is to open the local folder and let the cmake integration handle it. To run the application you'll need to select the /bin/Sandbox.exe startup project. (TODO: screenshots)

If you want to run this manually in the terminal: from the root dir ```mkdir build```
Navigate to the build dir: ```cd build``` and run the cmake config command ```cmake .. --preset=sandbox-debug``` to configure the build for the debug preset. Reference CMakePresets.json for --preset flag options.

If config completes normally, run the build command: ```cmake --build .```

# Using the Editor
TODO
