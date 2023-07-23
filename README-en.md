![LunarYue Logo](source/editor/editor_runtime/resource/LunarYueEngine.png)

This README file is available in multiple languages:

- [日本語](README.md)
- [中文](README-zh.md)

## Prerequisites

To build LunarYue, you need to first install the following tools:

### Windows 10/11
- Visual Studio 2019 (or later versions)
- CMake 3.19 (or later versions)
- Git 2.1 (or later versions)

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
</details>

## Build

### Windows
You can run the build_windows.bat. This batch file will automatically generate the project and build the Release configuration of LunarYue Engine. Upon successful build, the build/bin directory will contain the executable LunarYueEditor.

Alternatively, you can use the following command in the root directory of the project to generate the project in the build folder:
```
cmake -B build
```
Compile the project with Release. If you want to debug the project, use Debug:
```
cmake --build build --config Release
```

### Ubuntu


## Libraries
[bgfx](https://github.com/bkaradzic/bgfx) (Rendering)

[cereal](https://github.com/USCiLab/cereal) (Serialization)

[rttr](https://github.com/rttrorg/rttr) (Runtime Type Reflection)

[spdlog](https://github.com/gabime/spdlog) (Logging)

[imgui](https://github.com/ocornut/imgui) (Gui)

[assimp](https://github.com/assimp/assimp) (3D Model Import)

[glm](https://github.com/g-truc/glm) (Math Library)

[openal-soft](https://github.com/kcat/openal-soft) (3D Audio)
