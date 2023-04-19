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

# Vulkanのヘッダーファイルのディレクトリを設定する
set(vulkan_include ${THIRD_PARTY_DIR}/VulkanSDK/include)


if(WIN32)
    set(vulkan_lib ${THIRD_PARTY_DIR}/VulkanSDK/lib/Win32/vulkan-1.lib)  # Win32プラットフォームの場合、vulkan-1.libを使用する
    set(glslangValidator_executable ${THIRD_PARTY_DIR}/VulkanSDK/bin/Win32/glslangValidator.exe)  # Win32プラットフォームの場合、glslangValidator.exeを使用する
    add_compile_definitions("LunarYue_VK_LAYER_PATH=${THIRD_PARTY_DIR}/VulkanSDK/bin/Win32")  # Win32プラットフォームの場合、VK_LAYER_PATHにWin32用のパスを追加する
elseif(UNIX)
    if(APPLE)
      set(vulkan_lib ${THIRD_PARTY_DIR}/VulkanSDK/lib/MacOS/libvulkan.1.dylib)  # MacOSプラットフォームの場合、libvulkan.1.dylibを使用する
      set(glslangValidator_executable ${THIRD_PARTY_DIR}/VulkanSDK/bin/MacOS/glslangValidator)  # MacOSプラットフォームの場合、glslangValidatorを使用する
      add_compile_definitions("LunarYue_VK_LAYER_PATH=${THIRD_PARTY_DIR}/VulkanSDK/bin/MacOS")  # MacOSプラットフォームの場合、VK_LAYER_PATHにMacOS用のパスを追加する
      add_compile_definitions("LunarYue_VK_ICD_FILENAMES=${THIRD_PARTY_DIR}/VulkanSDK/bin/MacOS/MoltenVK_icd.json")  # MacOSプラットフォームの場合、VK_ICD_FILENAMESにMacOS用のパスを追加する
    else()
      set(vulkan_lib ${THIRD_PARTY_DIR}/VulkanSDK/lib/Linux/libvulkan.so.1)  # Linuxプラットフォームの場合、libvulkan.so.1を使用する
      set(glslangValidator_executable ${THIRD_PARTY_DIR}/VulkanSDK/bin/Linux/glslangValidator)  # Linuxプラットフォームの場合、glslangValidatorを使用する
      add_compile_definitions("LunarYue_VK_LAYER_PATH=${THIRD_PARTY_DIR}/VulkanSDK/bin/Linux")  # Linuxプラットフォームの場合、VK_LAYER_PATHにLinux用のパスを追加する
    endif()
else()
  message(FATAL_ERROR "Unknown Platform")  # 未知のプラットフォームの場合、エラーメッセージを出力する
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
# source/testフォルダーのビルドを実行しない
# add_subdirectory(source/test)

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

