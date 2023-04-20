# LunarYue Engine

<p align="center">
    <img src="engine/source/editor/resource/LunarYueEngine.png" width="400" alt="LunarYue Engineのロゴ">
  </a>
</p>

LunarYueEngineは、C++とVulkanをベースにしたクロスプラットフォームのゲームエンジンです。現代の3Dゲームやアプリケーションを作成するための、強力で高性能なグラフィックスレンダリング機能を提供することを目指しています。

## 主な機能

- Vulkanをベースにした高性能レンダリング
- クロスプラットフォーム対応（Windows、macOS、Linux）
- エンティティコンポーネントシステム（ECS）アーキテクチャ
- 組み込みエディタ
- 物理デバッグレンダラー（Windowsのみ）
- 柔軟なリソース管理

## 言語

このREADMEファイルは複数の言語で利用可能です。

- [English](README-en.md)
- [日本語](README.md)
- [中文](README-zh.md)

## 前提条件

LunarYueをビルドするには、まず以下のツールをインストールする必要があります。

### Windows 10/11
- Visual Studio 2019（またはそれ以降のバージョン）
- CMake 3.19（またはそれ以降のバージョン）
<details>
  <summary>CMakeインストール手順</summary>
  

  1. CMakeの公式サイト (https://cmake.org/download/) にアクセスして、Windows向けのインストーラーをダウンロードします。

  2. ダウンロードしたインストーラーを実行し、CMakeをインストールします。インストール時に、「Add CMake to the system PATH for all users」オプションを選択して、CMakeがシステムのPATHに追加されるようにしてください。

  3. インストールが完了したら、コマンドプロンプトを開いて、'cmake --version'コマンドを実行して、CMakeが正しくインストールされていることを確認します。このコマンドが実行されると、CMakeのバージョン情報が表示されます。

</details>

- Git 2.1（またはそれ以降のバージョン）

### macOS >= 10.15 (x86_64)
- Xcode 12.3（またはそれ以降のバージョン）
- CMake 3.19（またはそれ以降のバージョン）
- Git 2.1（またはそれ以降のバージョン）

### Ubuntu 20.04
- 以下のパッケージをインストールします
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
  <summary><b>CMakeインストール手順</b></summary>
  <br>
<b>Windows:</b>

1. CMakeの公式サイト (https://cmake.org/download/) にアクセスして、Windows向けのインストーラーをダウンロードします。

2. ダウンロードしたインストーラーを実行し、CMakeをインストールします。インストール時に、「Add CMake to the system PATH for all users」オプションを選択して、CMakeがシステムのPATHに追加されるようにしてください。

3. インストールが完了したら、コマンドプロンプトを開いて、'cmake --version'コマンドを実行して、CMakeが正しくインストールされていることを確認します。このコマンドが実行されると、CMakeのバージョン情報が表示されます。

  <br>
<b>Linux:</b>

1. ターミナルを開いて、以下のコマンドを実行してCMakeをインストールします。

``` sql
Copy code
sudo apt-get update
sudo apt-get install cmake
```

2. インストールが完了したら、'cmake --version'コマンドを実行して、CMakeが正しくインストールされていることを確認します。このコマンドが実行されると、CMakeのバージョン情報が表示されます。

  <br>
<b>macOS:</b>

1. Homebrewがインストールされていない場合は、公式サイト (https://brew.sh/index_ja) の指示に従ってインストールします。

2. ターミナルを開いて、以下のコマンドを実行してCMakeをインストールします。

```
brew install cmake
```
3. インストールが完了したら、'cmake --version'コマンドを実行して、CMakeが正しくインストールされていることを確認します。このコマンドが実行されると、CMakeのバージョン情報が表示されます。

  <br>
</details>

## LunarYueのビルド

### Windowsでのビルド
**build_windows.bat** を実行することができます。このバッチファイルは、プロジェクトを自動的に生成し、**LunarYue Engine** の **Release** 構成をビルドします。ビルドが成功すると、**bin** ディレクトリに **LunarYueEditor.exe** が作成されます。

また、次のコマンドを使用して最初に **Visual Studio** プロジェクトを生成し、ビルドディレクトリ内のソリューションを開いて手動でビルドすることができます。

### macOSでのビルド

> 以下のビルド手順は、x86_64の特定のハードウェアでのみテストされており、M1チップには対応していません。M1対応については後日リリース予定です。

LunarYueをコンパイルするには、最新版のXcodeがインストールされている必要があります。
次に、プロジェクトのルートディレクトリから 'cmake' を実行して、Xcodeのプロジェクトを生成します。

```
cmake -S . -B build -G "Xcode"
```
そして、次のコマンドでプロジェクトをビルドできます。
```
cmake --build build --config Release
```

また、build_macos.sh を実行してバイナリをビルドすることもできます。

### Ubuntu 20.04でビルド
**build_linux.sh**を実行してバイナリをビルドできます。

## ドキュメント
ドキュメントについては、[Wiki](https://github.com/wuyukwi/LunarYueEngine/wiki)セクションを参照してください。

## 追加情報

### Vulkan Validation Layer：検証エラー
LunarYueEditor.exeが正常に実行されるものの、デバッグ中にVulkan Validation Layer：Validation Errorという例外が報告されることがあることに気付きました。この問題は、Vulkan SDK（公式の最新バージョンで構いません）をインストールすることで解決できます。

### コンパイルデータベースの生成
`Unix Makefiles`ジェネレータが利用可能な場合、次のコマンドで`compile_commands.json`をビルドできます。`compile_commands.json`は`clangd`言語サーバに必要なファイルで、Emacsのcpp lsp-modeのバックエンドです。

Windowsの場合：

``` powershell
cmake -DCMAKE_TRY_COMPILE_TARGET_TYPE="STATIC_LIBRARY" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -S . -B compile_db_temp -G "Unix Makefiles"
copy compile_db_temp\compile_commands.json .
```

### Physics Debug Rendererの使用
現在、Physics Debug RendererはWindowsでのみ利用可能です。デバッガプロジェクトを含むソリューションを生成するには、次のコマンドを使用します。

``` powershell
cmake -S . -B build -DENABLE_PHYSICS_DEBUG_RENDERER=ON
```

注：
1. ソリューションを再生成する前に、ビルドディレクトリをクリーンしてください。以前のCMakeCacheを使用して直接再生成する際に、ビルドの問題が発生したことがあります。
2. Physics Debug Rendererは、LunarYueEditorを起動すると実行されます。両方のシーンのカメラ位置は同期されています。ただし、Physics Debug Rendererの初期カメラモードが間違っています。マウスホイールを1回下にスクロールすると、Physics Debug Rendererのカメラが正しいモードに変わります。

