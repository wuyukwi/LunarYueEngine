# LunarYue Engine

<p align="center">
    <img src="engine/source/editor/resource/LunarYueEngine.png" width="400" alt="LunarYue Engineのロゴ">
  </a>
</p>

LunarYueEngine 是一个基于 C++ 和 Vulkan 的跨平台游戏引擎。它旨在提供用于创建现代 3D 游戏和应用程序的强大和高性能的图形渲染能力。

## 主要功能

- 基于 Vulkan 的高性能渲染
- 跨平台支持（Windows, macOS, Linux）
- Entity-Component-System（ECS）架构
- 内置编辑器
- 物理调试渲染器（仅限 Windows）
- 灵活的资源管理

## 语言

此README文件有多种语言版本。

- [英语](README-en.md)
- [日语](README.md)
- [中文](README-zh.md)


## 预备条件

要构建LunarYue，您首先需要安装以下工具：

### Windows 10/11
- Visual Studio 2019（或更高版本）
- CMake 3.19（或更高版本）
- Git 2.1（或更高版本）

### macOS >= 10.15 (x86_64)
- Xcode 12.3（或更高版本）
- CMake 3.19（或更高版本）
- Git 2.1（或更高版本）

### Ubuntu 20.04
- 安装以下软件包
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
## 构建LunarYue

### 在Windows上构建
您可以运行 **build_windows.bat**。此批处理文件会自动生成项目并构建 **LunarYue Engine** 的 **Release** 配置。构建成功后，**bin** 目录中将创建 **LunarYueEditor.exe**。

另外，您还可以使用以下命令首先生成 **Visual Studio** 项目，然后在构建目录中打开解决方案并手动构建。

### 在macOS上构建

> 以下构建步骤仅在特定的x86_64硬件上进行了测试，不支持M1芯片。关于M1支持将在后续版本中发布。

要编译LunarYue，您需要安装最新版本的Xcode。
接下来，从项目根目录运行 'cmake' 以生成Xcode项目。

```
cmake -S . -B build -G "Xcode"
```
然后，您可以使用以下命令构建项目。
```
cmake --build build --config Release
```


另外，您还可以运行 build_macos.sh 来构建二进制文件。

### 在Ubuntu 20.04上构建
您可以运行 **build_linux.sh** 来构建二进制文件。

## 文档
有关文档，请参阅Wiki部分。

## 附加信息

### Vulkan验证层：验证错误
尽管LunarYueEditor.exe能正常运行，但在调试过程中可能会报告Vulkan验证层：验证错误这个异常。通过安装Vulkan SDK（官方最新版本即可）可以解决此问题。

### 生成编译数据库
如果 `Unix Makefiles` 生成器可用，您可以使用以下命令构建 `compile_commands.json`。`compile_commands.json` 是 `clangd` 语言服务器所需的文件，是Emacs的cpp lsp-mode的后端。

对于Windows：

``` powershell
cmake -DCMAKE_TRY_COMPILE_TARGET_TYPE="STATIC_LIBRARY" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -S . -B compile_db_temp -G "Unix Makefiles"
copy compile_db_temp\compile_commands.json .
```

### 使用物理调试渲染器
目前，物理调试渲染器仅在Windows上可用。要生成包含调试器项目的解决方案，请使用以下命令：

```
cmake -S . -B build -DENABLE_PHYSICS_DEBUG_RENDERER=ON
```

注意：
1. 在重新生成解决方案之前，请清理构建目录。在直接使用以前的CMakeCache重新生成时，曾发生过构建问题。
2. 当启动LunarYueEditor时，物理调试渲染器将运行。两个场景的摄像机位置是同步的。然而，物理调试渲染器的初始摄像机模式是错误的。向下滚动鼠标滚轮一次，物理调试渲染器的摄像机将切换到正确的模式。


