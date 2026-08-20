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

# ---- Vendored dependency compatibility ----
# rttr (vendored in source/3rdparty) declares cmake_minimum_required(VERSION 3.0).
# CMake 3.27+ deprecates and CMake 4.x removes support for minimums below 3.5,
# which fails the configure. Allow a policy floor so the vendored dependency
# keeps configuring on modern CMake.
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.27)
  set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
endif()

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  # Update if necessary
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=implicit-const-int-float-conversion")
endif()


set(ASSET_DIR "data")
add_subdirectory(source)
