# LunarYue Engine

<p align="center">
    <img src="engine/source/editor/resource/LunarYueEngine.png" width="400" alt="LunarYue Engineのロゴ">
  </a>
</p>

LunarYueEngine is a cross-platform game engine based on C++ and Vulkan. It aims to provide powerful and high-performance graphics rendering capabilities for creating modern 3D games and applications.

## Main Features

- High-performance rendering based on Vulkan
- Cross-platform support (Windows, macOS, Linux)
- Entity-Component-System (ECS) architecture
- Built-in editor
- Physics Debug Renderer (Windows only)
- Flexible resource management

## Language

This README file is available in multiple languages.

- [English](README-en.md)
- [Japanese](README.md)
- [Chinese](README-zh.md)

## Prerequisites

To build LunarYue, you need to first install the following tools:

### Windows 10/11
- Visual Studio 2019 (or later versions)
- CMake 3.19 (or later versions)
- Git 2.1 (or later versions)

### macOS >= 10.15 (x86_64)
- Xcode 12.3 (or later versions)
- CMake 3.19 (or later versions)
- Git 2.1 (or later versions)

### Ubuntu 20.04
- Install the following packages
```
sudo apt install libxrandr-dev
sudo apt install libxrender-dev
sudo apt install libxinerama-dev
sudo apt install libxcursor-dev
sudo apt install libxi-dev
sudo apt install libglvnd-dev
sudo apt install libvulkan-dev
sudo apt install cmake
sudo apt install clang
sudo apt install libc++-dev
sudo apt install libglew-dev
sudo apt install libglfw3-dev
sudo apt install vulkan-validationlayers
sudo apt install mesa-vulkan-drivers
```

<details>
  <summary><b>CMake Installation Guide</b></summary>
  <br>
 <b>Windows:</b>

1. Visit the CMake official website (https://cmake.org/download/) and download the installer for Windows.

2. Run the downloaded installer and install CMake. During the installation, select the "Add CMake to the system PATH for all users" option to add CMake to the system PATH.

3. After the installation is complete, open the command prompt and run the 'cmake --version' command to check if CMake has been correctly installed. After running this command, the version information of CMake will be displayed.

  <br>
<b>Linux:</b>

1. Open the terminal and run the following command to install CMake:

   ```
   sudo apt-get update
   sudo apt-get install cmake
   ```

2. After the installation is complete, run the 'cmake --version' command to check if CMake has been correctly installed. After running this command, the version information of CMake will be displayed.

  <br>
<b>macOS:</b>

1. If you haven't installed Homebrew yet, follow the instructions on the official website (https://brew.sh/index_en).

2. Open the terminal and run the following command to install CMake:

   ```
   brew install cmake
   ```

3. After the installation is complete, run the 'cmake --version' command to check if CMake has been correctly installed. After running this command, the version information of CMake will be displayed.

  <br>
</details>

## Building LunarYue

### Building on Windows
You can run the **build_windows.bat**. This batch file automatically generates the project and builds the **Release** configuration of the **LunarYue Engine**. Upon successful build, the **LunarYueEditor.exe** will be created in the **bin** directory.

Alternatively, you can first generate the **Visual Studio** project using the following command and manually build the solution inside the build directory:

### Building on macOS

> The build instructions below have been tested only on specific x86_64 hardware and do not support M1 chips. Support for M1 chips will be released at a later date.

To compile LunarYue, you need the latest version of Xcode installed.
Next, run 'cmake' from the root directory of the project to generate the Xcode project.

```
cmake -S . -B build -G "Xcode"

```
Then, you can build the project with the following command:

```
cmake --build build --config Release
```


Alternatively, you can run the build_macos.sh to build the binary.

### Building on Ubuntu 20.04
You can run the **build_linux.sh** to build the binary.

## Documentation
Please refer to the Wiki section for documentation.

## Additional Information

### Vulkan Validation Layer: Validation Errors
on Windows have noticed that LunarYueEditor.exe runs fine, but they encounter an exception called Vulkan Validation Layer: Validation Error during debugging. This issue can be resolved by installing the Vulkan SDK (the official latest version is fine).

### Generating compile database
If the `Unix Makefiles` generator is available, you can build the `compile_commands.json` with the following command. `compile_commands.json` is a file required by the `clangd` language server, which is the backend for Emacs's cpp lsp-mode.

For Windows:

``` powershell
cmake -DCMAKE_TRY_COMPILE_TARGET_TYPE="STATIC_LIBRARY" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -S . -B compile_db_temp -G "Unix Makefiles"
copy compile_db_temp\compile_commands.json .
```

### Using Physics Debug Renderer
Currently, the Physics Debug Renderer is available only on Windows. To generate a solution with the debugger project included, use the following command:

``` powershell
cmake -S . -B build -DENABLE_PHYSICS_DEBUG_RENDERER=ON

```
Note:

1. Please clean the build directory before regenerating the solution. There have been instances where build issues occurred when regenerating directly using a previous CMakeCache.
2. The Physics Debug Renderer runs when the LunarYueEditor is launched. The camera positions for both scenes are synchronized. However, the initial camera mode of the Physics Debug Renderer is incorrect. Scroll the mouse wheel down once, and the camera of the Physics Debug Renderer will switch to the correct mode.




