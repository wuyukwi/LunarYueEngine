![LunarYue Logo](source/editor/editor_runtime/resource/LunarYueEngine.png)

このREADMEファイルは複数の言語で利用可能です。

- [English](README-en.md)
- [中文](README-zh.md)


## 注意事項
Ubuntuでのビルド問題はまだ解決中です。現時点ではWindowでのビルドをお願いします。

## 前提条件

LunarYueEngineをビルドするには、まず以下のツールをインストールする必要があります。

### Windows 10/11
- Visual Studio 2019（またはそれ以降のバージョン）
- CMake 3.19（またはそれ以降のバージョン）
- Git 2.1（またはそれ以降のバージョン）

<details>
  <summary><b>CMakeインストール手順</b></summary>
  <br>
<b>Windows:</b>

1. CMakeの公式サイト (https://cmake.org/download/) にアクセスして、Windows向けのインストーラーをダウンロードします。

2. ダウンロードしたインストーラーを実行し、CMakeをインストールします。インストール時に、「Add CMake to the system PATH for all users」オプションを選択して、CMakeがシステムのPATHに追加されるようにしてください。

3. インストールが完了したら、コマンドプロンプトを開いて、'cmake --version'コマンドを実行して、CMakeが正しくインストールされていることを確認します。このコマンドが実行されると、CMakeのバージョン情報が表示されます。

  <br>
<b>Ubuntu:</b>

1. ターミナルを開いて、以下のコマンドを実行してCMakeをインストールします。

``` sql
Copy code
sudo apt-get update
sudo apt-get install cmake
```

2. インストールが完了したら、'cmake --version'コマンドを実行して、CMakeが正しくインストールされていることを確認します。このコマンドが実行されると、CMakeのバージョン情報が表示されます。

  <br>
</details>

## ビルド

### Windows
Gitからクローンします。パスには英語以外の文字を使用しないでください。
```
git clone https://github.com/wuyukwi/LunarYueEngine
```
フォルダ内に移動します。
```
cd LunarYueEngine
```

build_windows.batを実行することができます。このバッチファイルはプロジェクトを自動生成し、LunarYueEngineの Debug構成をビルドします。
ビルドが成功すると、build/binディレクトリに実行可能なLunarYueEditorが作成されます。
```
.\build_windows.bat
```

また、以下のコマンドを使用して、プロジェクトのルートディレクトリにプロジェクトを生成することも可能です。
```
cmake -B build
```
Releaseでプロジェクトをコンパイルします。
```
cmake --build build --config Release
```
プロジェクトをデバッグする場合はDebugを使用してください。
```
cmake --build build --config Debug
```

### Ubuntu


## 外部ライブラリ
[bgfx](https://github.com/bkaradzic/bgfx) (レンダリング)

[cereal](https://github.com/USCiLab/cereal) (シリアライゼーション)

[rttr](https://github.com/rttrorg/rttr) (ランタイム型リフレクション)

[spdlog](https://github.com/gabime/spdlog) (ログ)

[imgui](https://github.com/ocornut/imgui) (Gui)

[assimp](https://github.com/assimp/assimp) (3Dモデルのインポート)

[glm](https://github.com/g-truc/glm) (数学ライブラリ)

[openal-soft](https://github.com/kcat/openal-soft) (3D オーディオ)

# エディタ操作ガイド

## 1. 操作モードの切り替え
- `W` キーを押すと、操作モードが「平行移動」になります。
- `E` キーを押すと、操作モードが「回転」になります。
- `R` キーを押すと、操作モードが「スケーリング」になります。

## 2. 座標系の切り替え
- `T` キーを押すと、座標系が「ローカル座標系」になります。これは、スケーリング操作のときのデフォルトの座標系です。
- `Y` キーを押すと、座標系が「ワールド座標系」になります。ただし、スケーリング操作のときには利用できません。

## 3. オブジェクトの操作
- オブジェクトを選択すると、操縦杆が表示され、マウスを使ってオブジェクトを操作することができます。
- 右マウスボタンを押した状態で操縦杆を操作すると、オブジェクトを移動、回転、スケーリングすることができます。

## 4. スナップ（一定間隔での移動/回転/スケーリング）
- `LControl` キーを押しながらオブジェクトを操作すると、一定間隔（スナップ）で移動、回転、スケーリングが可能になります。

## 5. カメラの操作
- 編集ウィンドウがフォーカスされていなければ、カメラの操作は無効になります。

### 5.1 マウス操作
- `右マウスボタン`を押している間、カメラの角度をマウスの動きに従って変更できます。
- `中マウスボタン`を押している間、マウスの動きに応じてカメラを平行移動できます。

### 5.2 キーボード操作
- `右マウスボタン`を押している間、以下のキーボードの操作が可能になります：
  - `W` / `S` キー：カメラを前後に移動します。
  - `A` / `D` キー：カメラを左右に移動します。
  - `Space` / `LControl` キー：カメラを上下に移動します。
  - `上矢印` / `下矢印` キー：カメラを前後に移動します。
  - `左矢印` / `右矢印` キー：カメラを左右に移動します。
  - `マウスホイール`：カメラを前後に移動します。

- すべての移動操作は、`LShift` キーを押すことでスピードアップできます。


