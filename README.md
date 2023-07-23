![LunarYue Logo](engine/source/editor/resource/LunarYueEngine.png)


## 主な特徴



## 言語

このREADMEファイルは複数の言語で利用可能です。

- [English](README-en.md)
- [中文](README-zh.md)

## 前提条件

LunarYueをビルドするには、まず以下のツールをインストールする必要があります。

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
</details>

### Windows 10/11
- Visual Studio 2019（またはそれ以降のバージョン）
- CMake 3.19（またはそれ以降のバージョン）
- Git 2.1（またはそれ以降のバージョン）

## LunarYueのビルド

### Windowsでのビルド
**build_windows.bat** を実行することができます。このバッチファイルは、プロジェクトを自動的に生成し、**LunarYue Engine** の **Release** 構成をビルドします。ビルドが成功すると、**bin** ディレクトリに **LunarYueEditor.exe** が作成されます。

また、次のコマンドを使用して最初に **Visual Studio** プロジェクトを生成し、ビルドディレクトリ内のソリューションを開いて手動でビルドすることができます。

```
cmake -S . -B build
```

### Ubuntu 20.04でビルド

## ドキュメント

詳細なドキュメントは、[Wiki](https://github.com/wuyukwi/LunarYueEngine/wiki)ページで確認できます。

## ライセンス

LunarYueEngineは、MITライセンスの下で公開されています。詳細については、[LICENSE](LICENSE)ファイルを参照してください。

## コントリビューション

プルリクエストやフィードバックは大歓迎です。問題を報告する場合は、[Issue](https://github.com/wuyukwi/LunarYueEngine/issues)ページで新しい問題を作成してください。

## 追加情報

