![LunarYue Logo](source/editor/editor_runtime/resource/LunarYueEngine.png)

# LunarYueEngine

A 3D game engine written in C++17 with a dock-based visual editor, built on [bgfx](https://github.com/bkaradzic/bgfx). Build and runtime are verified on Windows.

- [日本語](README.md)

## Features

- **Deferred rendering pipeline** (PBR, reflection probes, LOD)
- **ECS architecture** (transform / camera / light / model / audio components)
- **Runtime reflection + serialization** (rttr + cereal — editor UI is generated from reflected types)
- **Asset pipeline** (models / textures / shaders compiled inside the editor)
- **Dock-based editor** (scene / hierarchy / inspector / console / project)
- **3D audio** (OpenAL Soft)
- **Animation** (skeletal animation, bone system)

## Build requirements

- Windows 10/11
- Visual Studio 2019 or newer
- CMake 3.19 or newer
- Git

## Build

```bat
git clone https://github.com/wuyukwi/LunarYueEngine
cd LunarYueEngine
cmake -B build
cmake --build build --config Debug
```

The executable is generated at `build/bin/Debug/LunarYueEditor.exe`.

> Previously, CMake 4.x required `-DCMAKE_POLICY_VERSION_MINIMUM=3.5` to configure; this is no longer needed (the root CMakeLists.txt handles it automatically).

## Editor controls

| Action | Key |
|--------|-----|
| Translate / Rotate / Scale modes | `W` / `E` / `R` |
| Local / World coordinate system | `T` / `Y` |
| Manipulate object | Select, then drag the gizmo with the mouse |
| Snap (incremental movement) | Hold `LControl` while manipulating |
| Camera orbit / pan / zoom | Right mouse / middle mouse / wheel |
| Camera move (while holding right mouse) | `W` `S` `A` `D` `Space` `LControl`; hold `LShift` to speed up |
| Delete / Duplicate / Focus entity | `Delete` / `Ctrl+D` / `Shift+F` |

## Supported asset formats

- Textures: `png` `tga` `dds` `ktx` `pvr`
- Meshes: `obj` `fbx` `dae` `3ds`
- Audio: `ogg` `wav`

## Third-party libraries

[bgfx](https://github.com/bkaradzic/bgfx) (rendering) / [cereal](https://github.com/USCiLab/cereal) (serialization) / [rttr](https://github.com/rttrorg/rttr) (reflection) / [spdlog](https://github.com/gabime/spdlog) (logging) / [imgui](https://github.com/ocornut/imgui) (GUI) / [assimp](https://github.com/assimp/assimp) (model import) / [glm](https://github.com/g-truc/glm) (math) / [openal-soft](https://github.com/kcat/openal-soft) (audio) / [nativefiledialog](https://github.com/mlabbe/nativefiledialog) (native dialogs) / [stb](https://github.com/nothings/stb)

All dependencies are vendored under `source/3rdparty`.

## Project layout

```
source/
├── engine/
│   ├── core/      # Foundation libraries (graphics / math / logging / serialization / tasks — 18 modules)
│   └── runtime/   # Game runtime (ECS / assets / rendering / animation / input)
├── editor/
│   ├── editor_core/     # GUI / mesh import / native file dialogs
│   └── editor_runtime/  # Editor application (docks / asset pipeline / main.cpp)
└── 3rdparty/      # Vendored dependencies
```

See [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) for details.

## Known limitations

- **Ubuntu builds are untested** (only Windows + VS2019/VS2022 have been verified)
- Recompiling shaders on the OpenGL backend may be affected by a platform-detection issue (no impact on the default Direct3D 11 backend)
- Running multiple editor instances is unsupported (a second instance may hit a runtime error)

## License

[MIT](LICENSE)
