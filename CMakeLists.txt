# このプロジェクトがCMakeのバージョン3.19以上が必要であることを指定しています。もしバージョンが満たされていない場合、FATAL_ERRORオプションによってエラーメッセージが表示され、ビルドが停止されます。
cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

project(LunarYue VERSION 0.1.0)

# C++の標準バージョンをC++17に設定しています。
set(CMAKE_CXX_STANDARD 17)
# C++17が必須であることを示しています。
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# デフォルトで共有ライブラリのビルドを無効にしています。
set(BUILD_SHARED_LIBS OFF)

# 条件付きのオプションを処理するためのCMakeモジュールをインクルードしています。
include(CMakeDependentOption)

# ---- Include guards ----
# ソースディレクトリとバイナリディレクトリが同じ場合に真となる条件をチェックしています。これは、インソースビルドを禁止するためのものです。
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
    FATAL_ERROR
      "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
  )
endif()

# プロジェクトのルートディレクトリをCMakeの現在のソースディレクトリとして設定しています。
set(LunarYue_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# インストール先のプレフィックスをプロジェクトのbinディレクトリに設定しています。
set(CMAKE_INSTALL_PREFIX "${LunarYue_ROOT_DIR}/bin")

# バイナリのルートディレクトリをインストールプレフィックスに設定しています。
set(BINARY_ROOT_DIR "${CMAKE_INSTALL_PREFIX}/")

add_subdirectory(engine)