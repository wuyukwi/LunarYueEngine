# LunarYueEngine Architecture

This document describes the high-level architecture of LunarYueEngine. It is a companion to the codebase — read it before diving into the source. Last verified: 2026-08 (Windows, VS2022, CMake 4.x, Debug build).

## Overview

LunarYueEngine is a 3D game engine with an integrated dock-based editor, written in C++17 (~95k lines of first-party code). Rendering is built on bgfx; windowing/input on a vendored SFML 2.x fork (renamed "mml"); reflection on rttr; serialization on cereal with a custom archive layer.

```
main.cpp (editor_runtime)
  └─ editor::app : runtime::app     ← application framework (setup/start/stop/run_one_frame)
       ├─ engine/core      (18 foundation libraries, aggregated as INTERFACE target)  ~41k LOC
       ├─ engine/runtime   (ECS / assets / rendering / animation / input)             ~25k LOC
       ├─ editor_core      (gui / mesh_import / nativefd)                             ~19k LOC
       └─ editor_runtime   (editor app / docks / asset pipeline)                      ~10k LOC
```

## Build system

- **CMake 3.19+**, single-project layout: root `CMakeLists.txt` → `source/` → `3rdparty`, `engine`, `editor`
- Static linking only (`BUILD_SHARED_LIBS OFF`, `ETH_STATIC`); MSVC builds use `/MP`
- All third-party dependencies are **vendored** under `source/3rdparty` (no submodules, no package manager, no version locks)
- Output goes to `bin/<Config>/` next to the build dir (`set_output_paths()` in `cmake/macros.cmake`)
- The editor's post-build step copies `data/` and `bgfx/src` next to the executable (runtime shader include path)
- `file(GLOB_RECURSE ... CONFIGURE_DEPENDS)` collects sources in most module CMakeLists — new files are picked up automatically on rebuild
- CMake 4.x note: the vendored rttr declares `cmake_minimum_required(VERSION 3.0)`; the root `CMakeLists.txt` sets `CMAKE_POLICY_VERSION_MINIMUM 3.5` so no CLI flag is needed

### Targets

| Target | Type | Links | Contents |
|--------|------|-------|----------|
| `core` | INTERFACE | — | aggregates the 18 `engine/core/*` module libs |
| `runtime` | static lib | `core`, `mml-window` | everything under `engine/runtime` |
| `editor_core` | static lib | `mesh_import`, `nativefd`, `gui` | editor GUI toolkit + import |
| `LunarYueEditor` | executable | `editor_core`, `runtime` | the editor application |

## Application lifecycle

Entry point: `source/editor/editor_runtime/main.cpp` — registers virtual filesystem protocols, then runs `editor::app` (a `runtime::app`).

```
runtime::app::run()
  ├─ core::details::initialize()
  ├─ setup(parser)          ← logging sinks, gfx/audio log bridges, ECS frame getter, CLI options
  ├─ start(parser)          ← register subsystems (order matters)
  └─ loop: run_one_frame() until running_ == false
       ├─ simulation tick → task system (5ms) → poll_events → process windows
       ├─ on_frame_begin / update / render / ui_render / end (virtual hooks)
```

### Subsystem registration (runtime::app::start)

```cpp
core::add_subsystem<core::simulation>();
core::add_subsystem<renderer>(parser);          // bgfx init
core::add_subsystem<input>();
core::add_subsystem<audio::device>();
core::add_subsystem<asset_manager>();
core::add_subsystem<core::task_system>(false);
core::add_subsystem<entity_component_system>();
core::add_subsystem<scene_graph>();
core::add_subsystem<bone_system>();
core::add_subsystem<camera_system>();
core::add_subsystem<reflection_probe_system>();
core::add_subsystem<deferred_rendering>();
core::add_subsystem<audio_system>();
```

The editor (`editor::app::start`) adds its own: `gui_system`, `docking_system`, `editing_system`, `picking_system`, `debugdraw_system`, `project_manager`.

**Subsystem pattern** (`engine/core/system/subsystem.h`): `add_subsystem<T>` / `get_subsystem<T>` — a type-indexed registry of singletons. This is the central wiring mechanism of the engine.

## Rendering

- **`gfx::` namespace** (`engine/core/graphics`) is a thin RAII wrapper over bgfx: `texture`, `frame_buffer`, `program`, `shader`, `uniform`, `vertex_buffer`, `index_buffer`, `render_view` + `debugdraw`
- **Deferred pipeline** (`engine/runtime/ecs/systems/deferred_rendering.h`, ~800 lines):
  - `gather_visible_models()` — ECS query (transform + model components), frustum culling, LOD selection, static/dynamic split
  - G-buffer pass (RGBA16F render targets) → light passes (directional/point/spot) → post-processing
  - Reflection probes + atmospheric shaders (`data/engine_data/data/shaders/`)
- Renderer backends: bgfx auto-selects (Direct3D 11 on Windows; feature level 12.1 verified)

## ECS

- **Not data-oriented**: components are stored as `std::vector<std::shared_ptr<component>>` per component type (`component_storage`), accessed via `chandle<T>` (weak_ptr) — simple, but with cache/size trade-offs
- Entities: `runtime/ecs/entity.hpp`; scene: `scene_graph` system manages hierarchy
- Components: `transform`, `camera`, `light`, `model`, `reflection_probe`, `audio_source`, `audio_listener`
- Systems (subsystems): `scene_graph`, `bone_system`, `camera_system`, `reflection_probe_system`, `deferred_rendering`, `audio_system`
- Serialization: scenes/prefabs serialize through the reflection layer

## Reflection & serialization

- Custom macros `REFLECT_EXTERN(T)` / `SAVE_EXTERN(T)` / `LOAD_EXTERN(T)` (in `engine/core/reflection/`) wrap rttr registration + custom save/load functions
- **Meta mirror directory**: `engine/runtime/meta/` holds generated-style `.hpp` glue headers (one per reflected type, e.g. `meta/ecs/components/transform_component.hpp`) that include the real header + register the type with cereal (`CEREAL_REGISTER_TYPE`)
- `runtime/meta/meta.h` includes every glue header; the editor includes it once in `main.cpp` — all registrations happen at startup
- Editor property panels are generated from reflection (`inspectors` in `editor_runtime/interface/`)
- ⚠️ When adding/renaming a reflected type: update the corresponding `.hpp` glue header or the build breaks at link/registration time

## Asset pipeline

- Editor compiles assets at runtime by shelling out to **`shaderc`** and **`texturec`** (bgfx tools — built automatically by the vendored bgfx CMake and placed next to the executable; see `editor_runtime/assets/asset_compiler.cpp`)
- Shader sources are `.sc` (GLSL, bgfx convention) with `.io` varying defs; compiled binaries ship pre-built in `data/engine_data/data/shaders/`
- Compilation targets are chosen by renderer backend (D3D11 → `s_4_0`, Vulkan → SPIR-V, OpenGL → GLSL 120)
  - ⚠️ Known issue: the OpenGL branch hardcodes platform "linux" — OpenGL-backend shader recompilation may fail on Windows (default D3D11 path unaffected)

## Virtual filesystem

Path protocols registered in `main.cpp`:

| Protocol | Maps to |
|----------|---------|
| `engine:` | `<binary>/data/engine_data` |
| `editor:` | `<binary>/data/editor_data` |
| `binary:` | executable directory |
| `shader_include:` | `<binary>/data/src` (bgfx shader chunks) |

Implemented in `engine/core/filesystem/` (`fs::path_protocol`, `resolve_protocol`, `add_path_protocol`).

## Logging & audio

- spdlog-based: `logging::` namespace, sink registry, `APPLOG_*` macros; gfx/audio log output is bridged into it
- Audio: OpenAL Soft wrapped in `engine/core/audio/` (`device`, `sound`); ECS components `audio_source` / `audio_listener`, `audio_system` updates positions

## Testing & CI

- **None at present.** No unit tests, no CI workflows. The only verification path is building and running the editor manually.
- Priority target: a smoke test (engine init → N frames → clean shutdown) runnable in CI.

## Working on the codebase

- Platform differences live **only inside `mml`** (the SFML fork) — first-party code has zero `_WIN32`/`__linux__` ifdefs
- `.clang-format` / `.clang-tidy` configs exist at the repo root
- Default branch is `dev`. Branches named `upgrade/*` / `feature/add-*` from 2023 are **broken experiments** (dependency upgrades that never worked) — do not merge them
- No per-module feature flags; adding a new `engine/core/*` module means: add a directory with a CMakeLists producing a library, then add it to the `DIRECTORIES` list in `engine/core/CMakeLists.txt`
