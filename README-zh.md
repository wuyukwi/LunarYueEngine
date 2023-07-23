![LunarYue Logo](source/editor/editor_runtime/resource/LunarYueEngine.png)

此README文件有多种语言版本

- [English](README-en.md)
- [日本語](README.md)

## 注意事项
在Ubuntu中的构建问题还在解决中，请暂时先使用Window来构建

## 预备条件

要构建LunarYueEngine，您首先需要安装以下工具：

### Windows 10/11
- Visual Studio 2019（或更高版本）
- CMake 3.19（或更高版本）
- Git 2.1（或更高版本）


### Ubuntu 
```
```

<details>
  <summary><b>CMake安装教程</b></summary>
  <br>
 <b>Windows：</b>

1. 访问CMake官方网站 (https://cmake.org/download/) ，下载适用于Windows的安装程序。

2. 运行下载的安装程序，安装CMake。在安装过程中，请选择**Add CMake to the system PATH for all users**选项，以便将CMake添加到系统的PATH中。

3. 安装完成后，打开命令提示符，运行'cmake --version'命令，检查CMake是否已正确安装。运行此命令后，将显示CMake的版本信息。

  <br>
<b>Ubuntu：</b>

1. 打开终端，运行以下命令以安装CMake：

   ```
   sudo apt-get update
   sudo apt-get install cmake
   ```

2. 安装完成后，运行'cmake --version'命令，检查CMake是否已正确安装。运行此命令后，将显示CMake的版本信息。

  <br>
<b>macOS：</b>

1. 如果尚未安装Homebrew，请按照官方网站 (https://brew.sh/index_ja) 的说明进行安装。

2. 打开终端，运行以下命令以安装CMake：

   ```
   brew install cmake
   ```

3. 安装完成后，运行'cmake --version'命令，检查CMake是否已正确安装。运行此命令后，将显示CMake的版本信息。

  <br>
</details>

## 构建
### Windows
您可以运行 **build_windows.bat**。此批处理文件会自动生成项目并构建 **LunarYue Engine** 的 **Release** 配置。
构建成功后，**build/bin** 目录中将创建可执行程序**LunarYueEditor**

另外，您还可以项目根目录使用以下命令来在build文件夹里生成项目
```
cmake -B build
```
使用Release来编译项目，如果要调试项目请使用Debug
```
cmake --build build --config Release
```

### Ubuntu
您可以运行 **build_linux.sh**。此批处理文件会自动生成项目并构建 **LunarYue Engine** 的 **Release** 配置。
构建成功后，**build/bin** 目录中将创建可执行程序**LunarYueEditor**

另外，您还可以项目根目录使用以下命令来在build文件夹里生成项目
```
cmake -B build
```
使用Release来编译项目，如果要调试项目请使用Debug
```
cmake --build build --config Release
```

## 第三方库
[bgfx](https://github.com/bkaradzic/bgfx) (多平台渲染)

[cereal](https://github.com/USCiLab/cereal) (序列化)

[rttr](https://github.com/rttrorg/rttr) (运行时类型反射)

[spdlog](https://github.com/gabime/spdlog) (日志)

[imgui](https://github.com/ocornut/imgui) (Gui)

[assimp](https://github.com/assimp/assimp) (导入3D模型)

[glm](https://github.com/g-truc/glm) (数学库)

[openal-soft](https://github.com/kcat/openal-soft) (3D 音频)

