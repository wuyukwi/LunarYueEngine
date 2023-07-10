set(ENGINE_ROOT_DIR "${LunarYue_ROOT_DIR}/engine")

# サードパーティライブラリのルートディレクトリ
set(THIRD_PARTY_DIR "${ENGINE_ROOT_DIR}/3rdparty")

# エンジンのアセットディレクトリ
set(ENGINE_ASSET_DIR "/asset")

# デプロイメント用の設定ファイルのディレクトリ
set(DEPLOY_CONFIG_DIR "configs/deployment")

# 開発用の設定ファイルのディレクトリ
set(DEVELOP_CONFIG_DIR "configs/development")

# 物理エンジンのデバッグレンダリングを有効にするかどうかのオプション
option(ENABLE_PHYSICS_DEBUG_RENDERER "Enable Physics Debug Renderer" OFF)

# Windows以外で物理エンジンのデバッグレンダリングを有効にしている場合、警告を出してオフにする
if(NOT WIN32)
  if(ENABLE_PHYSICS_DEBUG_RENDERER)
    message(WARNING "Disable Physics Debug Renderer")
    set(ENABLE_PHYSICS_DEBUG_RENDERER OFF CACHE BOOL "" FORCE)
  endif()
endif()

# 物理エンジンのデバッグレンダリングを有効にしている場合、アセットディレクトリを設定する
if(ENABLE_PHYSICS_DEBUG_RENDERER)
  set(JOLT_ASSET_DIR "/jolt-asset")
endif()

# MSVCの場合、並列ビルドのオプションを追加する
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options("/MP")
    set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT LunarYueEditor)
endif()

# LunarYueShaderCompileのターゲットを設定
set(SHADER_COMPILE_TARGET LunarYueShaderCompile)
# shaderフォルダーのビルドを実行
add_subdirectory(shader)
# 3rdpartyフォルダーのビルドを実行
add_subdirectory(3rdparty)
# source/runtimeフォルダーのビルドを実行
add_subdirectory(source/runtime)
# source/editorフォルダーのビルドを実行
add_subdirectory(source/editor)
# source/meta_parserフォルダーのビルドを実行
add_subdirectory(source/meta_parser)

# CODEGEN_TARGETを"LunarYuePreCompile"に設定
set(CODEGEN_TARGET "LunarYuePreCompile")
# source/precompile/precompile.cmakeファイルをインクルード
include(source/precompile/precompile.cmake)
# CODEGEN_TARGETのプロパティを設定
set_target_properties("${CODEGEN_TARGET}" PROPERTIES FOLDER "Engine" )
# LunarYueRuntimeターゲットに"${CODEGEN_TARGET}"の依存関係を追加
add_dependencies(LunarYueRuntime "${CODEGEN_TARGET}")
# "${CODEGEN_TARGET}"ターゲットに"LunarYueParser"の依存関係を追加
add_dependencies("${CODEGEN_TARGET}" "LunarYueParser")

