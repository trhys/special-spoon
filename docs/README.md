# Special-Spoon

<p align="center">
  <img width="250" height="250" alt="Special-Spoon Logo" src="https://github.com/user-attachments/assets/6fd33ac6-2b39-4a6d-9411-03c05ad28c5e" />
</p>

A modern, data-oriented application framework built on top of SFML and ImGui. This basically tucks away the SFML API and serialization behind a visual editor application. Intended usage includes games obviously, but should also support really any 2D windowed application as long as you're willing to design components and systems for it.

This framework is intentionally modularized to allow custom behavior to be written into it by the designer (write your own gameplay systems for instance). There is foundational, built-in components and systems to be used, but you can seamlessly plug in your own code and use it in-editor.

I intend to include packages of components and systems (fitted to some style of application; a combat system for a turn based rpg for example) as the project develops and matures further.

## Features

- **Data-Oriented ECS** - Efficient component-based entity management
- **SFML Integration** - Hardware-accelerated 2D rendering and multimedia capabilities
- **ImGui Editor** - Built-in visual editor for scene creation and management
- **JSON Serialization** - Scene persistence using Nlohmann JSON library
- **Cross-Platform** - Support for Windows, Linux, and other SFML-compatible platforms

## Stack

- **C/C++**
- **SFML** - 2D rendering and multimedia
- **ImGui** - GUI for editor
- **Nlohmann JSON** - Serialization
- **CMake** - Build

## Quick Start

### Prerequisites

- CMake 3.20 or higher
- A C++17 compatible compiler
- Git
- Python3 (recommended for automated build w/ dependencies)

### Building on Linux

The easiest way to get started on Linux is using the build script:

```bash
git clone https://github.com/trhys/special-spoon.git
cd special-spoon
sudo python3 buildme.py --preset sandbox-debug
```

The script will automatically handle dependencies, initialize submodules, and configure the build.

> **Note:** Depending on your Linux distribution, you may need to install additional packages manually. Detailed setup guide pending.

### Building on Windows

First - Clone the repository and initialize submodules:
```bash
git clone https://github.com/trhys/special-spoon.git
cd special-spoon
git submodule init
git submodule update
```

**Option A: Visual Studio with CMake Integration**
   - Open the cloned folder in Visual Studio
   - Let the CMake integration configure automatically
   - Select the appropriate startup project from `/bin/Sandbox`

**Option B: Command Line**
```bash
mkdir build
cd build
cmake .. --preset=sandbox-debug
cmake --build .
```

Refer to `CMakePresets.json` for available build presets.

## Project Structure

```
Special-Spoon/
├── src/
│   ├── ECS/              # Entity Component System
│   │   ├── Components/   # Component definitions
│   │   └── Systems/      # System implementations
│   ├── Core/             # Core framework
│   │   └── Registers/    # Action, State, and Component registries
│   ├── Editor/           # Editor interface
│   └── Application.h     # Main application class
├── CMakeLists.txt
├── CMakePresets.json
└── buildme.py            # Linux build helper script
```

## Usage

A full usage guide is pending --- at this time I'm still getting through alpha stage development so it doesn't make a lot of sense yet.

> **Note:** Eventually will include an imgui ini file to set a default window layout. Right now it's going to start you with basically no configuration but will remember where you put things on next load.

### Creating a Scene

Use the built-in editor to create scenes:

1. Launch the application
2. Create entities and attach components using the visual editor
3. Configure component properties through ImGui panels
4. Save your scene as JSON

### Loading a Scene

Scenes are saved as JSON and can be loaded programmatically through the EntityManager and component registry system.

## Development

### Contributing

Contributions are welcome! Please feel free to submit issues and pull requests.

## License

See LICENSE file for details.

## Resources

- [SFML Documentation](https://www.sfml-dev.org/documentation.php)
- [ImGui Repository](https://github.com/ocornut/imgui)
- [Nlohmann JSON](https://github.com/nlohmann/json)
