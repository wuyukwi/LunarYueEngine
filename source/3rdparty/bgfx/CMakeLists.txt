if(MSVC)
    add_compile_options(/wd4244 /wd4267 /wd4291)
endif()

if(MINGW)
    add_compile_definitions(ASTCENC_SSE=0)
endif()


#option(BX_AMALGAMATED "Amalgamated bx build for faster compilation" ON)
#option(BGFX_AMALGAMATED "Amalgamated bgfx build for faster compilation" ON)
#option(BGFX_BUILD_TOOLS "Build bgfx tools." ON)
#?cmake_dependent_option(BGFX_BUILD_TOOLS_BIN2C "Build bx binary to c converter." ON BGFX_BUILD_TOOLS OFF)
#cmake_dependent_option(BGFX_BUILD_TOOLS_BIN2C "Build bx binary to c converter." ON BGFX_BUILD_TOOLS OFF)
#cmake_dependent_option(BGFX_BUILD_TOOLS_SHADER "Build bgfx shader tools." ON BGFX_BUILD_TOOLS OFF)
#cmake_dependent_option(BGFX_BUILD_TOOLS_GEOMETRY "Build bgfx geometry tools." ON BGFX_BUILD_TOOLS OFF)
#cmake_dependent_option(BGFX_BUILD_TOOLS_TEXTURE "Build bgfx texture tools." ON BGFX_BUILD_TOOLS OFF)
#option(BGFX_INSTALL "Create installation target." ON)
option(BGFX_BUILD_EXAMPLES "Build bgfx examples." ON)

add_subdirectory(bgfx)
