![LunarYue Logo](source/editor/editor_runtime/resource/LunarYueEngine.png)

# LunarYueEngine

C++17 で書かれた 3D ゲームエンジンと、Dock ベースのビジュアルエディタを一体にしたプロジェクトです。レンダリングには [bgfx](https://github.com/bkaradzic/bgfx) を使用し、Windows でビルド・実行が確認されています。

- [English](README-en.md)

## 主な特徴

- **遅延レンダリングパイプライン**（PBR、リフレクションプローブ、LOD）
- **ECS アーキテクチャ**（transform / camera / light / model / audio などのコンポーネント）
- **ランタイムリフレクション + シリアライゼーション**（rttr + cereal、エディタ UI が自動生成される）
- **アセットパイプライン**（モデル / テクスチャ / シェーダをエディタ内でコンパイル）
- **Dock ベースのエディタ**（シーン / ヒエラルキー / インスペクタ / コンソール / プロジェクト）
- **3D オーディオ**（OpenAL Soft）
- **アニメーション**（スケルタルアニメーション、ボーンシステム）

## ビルド要件

- Windows 10/11
- Visual Studio 2019 以降
- CMake 3.19 以降
- Git

## ビルド

```bat
git clone https://github.com/wuyukwi/LunarYueEngine
cd LunarYueEngine
cmake -B build
cmake --build build --config Debug
```

ビルドが成功すると、`build/bin/Debug/LunarYueEditor.exe` が生成されます。

> 以前は `-DCMAKE_POLICY_VERSION_MINIMUM=3.5` を付けないと CMake 4.x で configure に失敗しましたが、現在は不要です（ルート CMakeLists.txt が自動処理します）。

## エディタの操作

| 操作 | キー |
|------|------|
| 移動 / 回転 / スケールモード | `W` / `E` / `R` |
| ローカル / ワールド座標系 | `T` / `Y` |
| オブジェクト操作 | 選択後にマウスでギズモをドラッグ |
| スナップ操作 | `LControl` を押しながら操作 |
| カメラ回転 / パン / ズーム | 右マウス / 中マウス / ホイール |
| カメラ移動（右マウス押下中） | `W` `S` `A` `D` `Space` `LControl`、`LShift` で高速化 |
| エンティティ削除 / 複製 / フォーカス | `Delete` / `Ctrl+D` / `Shift+F` |

## 対応アセットフォーマット

- テクスチャ: `png` `tga` `dds` `ktx` `pvr`
- メッシュ: `obj` `fbx` `dae` `3ds`
- オーディオ: `ogg` `wav`

## 外部ライブラリ

[bgfx](https://github.com/bkaradzic/bgfx)（レンダリング） / [cereal](https://github.com/USCiLab/cereal)（シリアライゼーション） / [rttr](https://github.com/rttrorg/rttr)（リフレクション） / [spdlog](https://github.com/gabime/spdlog)（ログ） / [imgui](https://github.com/ocornut/imgui)（GUI） / [assimp](https://github.com/assimp/assimp)（モデルインポート） / [glm](https://github.com/g-truc/glm)（数学） / [openal-soft](https://github.com/kcat/openal-soft)（オーディオ） / [nativefiledialog](https://github.com/mlabbe/nativefiledialog)（ネイティブダイアログ） / [stb](https://github.com/nothings/stb)

すべて `source/3rdparty` に vendored されています。

## プロジェクト構成

```
source/
├── engine/
│   ├── core/      # 基盤ライブラリ（グラフィックス / 数学 / ログ / シリアライズ / タスク等、18 モジュール）
│   └── runtime/   # ゲームランタイム（ECS / アセット / レンダリング / アニメーション / 入力）
├── editor/
│   ├── editor_core/     # GUI / メッシュインポート / ネイティブファイルダイアログ
│   └── editor_runtime/  # エディタ本体（Dock / アセットパイプライン / main.cpp）
└── 3rdparty/      # vendored 依存ライブラリ
```

詳細は [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) を参照してください。

## 既知の制限

- **Ubuntu ビルドは未検証**です（検証したのは Windows + VS2019/VS2022 のみ）
- OpenGL バックエンドでシェーダを再コンパイルする場合、プラットフォーム判定に問題がある可能性があります（既定の Direct3D 11 では影響なし）
- エディタの多重起動は想定されていません（2 つ目を起動するとランタイムエラーになる可能性があります）

## ライセンス

[MIT](LICENSE)
