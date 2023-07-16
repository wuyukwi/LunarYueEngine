set(third_party_folder "ThirdParty")

# 共有ライブラリをビルドしないようにする
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# glfwのビルドとプロパティ設定
#if(NOT TARGET glfw)
#    option(GLFW_BUILD_EXAMPLES "" OFF)
#    option(GLFW_BUILD_TESTS "" OFF)
#    option(GLFW_BUILD_DOCS "" OFF)
#    option(GLFW_INSTALL "" OFF)
#    add_subdirectory(glfw)
#    set_target_properties(glfw PROPERTIES FOLDER ${third_party_folder}/glfw)
#    set_target_properties(update_mappings PROPERTIES FOLDER ${third_party_folder}/glfw)
#endif()

# sdlのビルドとプロパティ設定
if(NOT TARGET SDL2)
    option(BGFX_WITH_SDL "" ON)
    option(SDL2_DISABLE_INSTALL "" ON)
    add_subdirectory(SDL2 EXCLUDE_FROM_ALL)

    set_target_properties(SDL2-static PROPERTIES FOLDER ${third_party_folder}/SDL2)
    set_target_properties(SDL2main PROPERTIES FOLDER ${third_party_folder}/SDL2)
    set_target_properties(SDL2_test PROPERTIES FOLDER ${third_party_folder}/SDL2)
    set_target_properties(sdl_headers_copy PROPERTIES FOLDER ${third_party_folder}/SDL2)
endif()

# spdlogのビルドとプロパティ設定
if(NOT TARGET spdlog)
    option(SPDLOG_BUILD_EXAMPLE "" OFF)
    option(SPDLOG_INSTALL "" OFF)
    add_subdirectory(spdlog)
    set_target_properties(spdlog PROPERTIES FOLDER ${third_party_folder}/spdlog)
endif()

# stbのビルドとプロパティ設定
if(NOT TARGET stb)
    include(stb.cmake)
    set_target_properties(stb PROPERTIES FOLDER ${third_party_folder}/stb)
endif()

# tinyobjloaderのビルドとプロパティ設定
if(NOT TARGET tinyobjloader)
    add_subdirectory(tinyobjloader)
    set_target_properties(tinyobjloader PROPERTIES FOLDER ${third_party_folder}/tinyobjloader)
    set_target_properties(uninstall PROPERTIES FOLDER ${third_party_folder}/tinyobjloader)
endif()

# Joltのビルドとプロパティ設定
if(NOT TARGET Jolt)
    option(TARGET_HELLO_WORLD "" OFF)
    option(TARGET_PERFORMANCE_TEST "" OFF)
    option(TARGET_SAMPLES "" OFF)
    option(TARGET_UNIT_TESTS "" OFF)
    option(TARGET_VIEWER "" OFF)

    # 物理デバッグレンダリングが有効な場合、テストフレームワークをビルドする
    if(ENABLE_PHYSICS_DEBUG_RENDERER)
        option(TARGET_TEST_FRAMEWORK "" ON)
    else()
        option(TARGET_TEST_FRAMEWORK "" OFF)
    endif()

    add_subdirectory(JoltPhysics/Build)

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        include(CheckCXXCompilerFlag)
        check_cxx_compiler_flag("-Wno-unqualified-std-cast-call" COMPILER_CHECK_UNQUALIFIED)
        if(COMPILER_CHECK_UNQUALIFIED)
            target_compile_options(Jolt PRIVATE "-Wno-unqualified-std-cast-call")
        endif()
    endif()

    # 物理デバッグレンダリングが有効な場合はTestFrameworkも含める
    if(ENABLE_PHYSICS_DEBUG_RENDERER)
        set_target_properties(Jolt TestFramework
            PROPERTIES
            FOLDER ${third_party_folder}/JoltPhysics
            MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
    else()
        set_target_properties(Jolt
            PROPERTIES
            FOLDER ${third_party_folder}/JoltPhysics
            MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
    endif()

endif()

# sol2のビルドとプロパティ設定
if(NOT TARGET sol2)
    add_subdirectory(sol2-3.3.0)
endif()

# luaのプロパティ設定
if(NOT TARGET lua)
    include(lua.cmake)
    set_target_properties(lua_static PROPERTIES FOLDER ${third_party_folder}/lua)
endif()

# assimpのビルドとプロパティ設定
if(NOT TARGET assimp)
    add_subdirectory(assimp)
    set_target_properties(assimp PROPERTIES FOLDER ${third_party_folder}/assimp)
	set_target_properties(UpdateAssimpLibsDebugSymbolsAndDLLs PROPERTIES FOLDER ${third_party_folder}/assimp)
	set_target_properties(zlibstatic PROPERTIES FOLDER ${third_party_folder}/assimp)
endif()

if(NOT TARGET bgfx)
    if(CMAKE_BUILD_TYPE MATCHES "Debug")
        option(BX_CONFIG_DEBUG "Log debug messages" ON)
        option(BGFX_CONFIG_DEBUG_ANNOTATION "Enable gfx debug annotations" ON)
    endif()
    option(BX_AMALGAMATED "Amalgamated bx build for faster compilation" ON)
    option(BGFX_AMALGAMATED "Amalgamated bgfx build for faster compilation" ON)
    option(BGFX_BUILD_TOOLS "Build bgfx tools." ON)
    cmake_dependent_option(BGFX_BUILD_TOOLS_BIN2C "Build bx binary to c converter." ON BGFX_BUILD_TOOLS OFF)
    cmake_dependent_option(BGFX_BUILD_TOOLS_SHADER "Build bgfx shader tools." ON BGFX_BUILD_TOOLS OFF)
    cmake_dependent_option(BGFX_BUILD_TOOLS_GEOMETRY "Build bgfx geometry tools." ON BGFX_BUILD_TOOLS OFF)
    cmake_dependent_option(BGFX_BUILD_TOOLS_TEXTURE "Build bgfx texture tools." OFF BGFX_BUILD_TOOLS OFF)
    option(BGFX_INSTALL "Create installation target." OFF)
    add_subdirectory(bgfx)
endif()

# imguiのビルドとプロパティ設定
#if(NOT TARGET imgui)
#   include(imgui.cmake)
#    set_target_properties(imgui PROPERTIES FOLDER ${third_party_folder}/imgui)
#endif()