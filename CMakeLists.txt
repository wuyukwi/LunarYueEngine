cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

project(LunarYue VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)

include(CMakeDependentOption)

# ---- Include guards ----
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
    FATAL_ERROR
      "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
  )
endif()

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  # Update if necessary
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=implicit-const-int-float-conversion")
endif()

set(ASSET_DIR "data")
set(EDITOR_NAME "LunarYueEditor")

add_subdirectory(source)
