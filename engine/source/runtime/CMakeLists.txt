#PackageProject.cmakeを使用して、ターゲットをインストール可能にします
set(TARGET_NAME LunarYueRuntime)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#json11を追加
set(JSON_INCLUDE ${THIRD_PARTY_DIR}/json11)
add_library(json11 ${JSON_INCLUDE}/json11.cpp)
set_target_properties(json11 PROPERTIES FOLDER "ThirdParty/json11")

#---- ソースファイルを追加 ----
#注意：ファイルを自動検出できないため、ファイルを変更する場合は明示的にここに記述してください。
file(GLOB_RECURSE HEADER_FILES "*.h")
file(GLOB_RECURSE SOURCE_FILES "*.cpp")

#ソースファイルをグループ化
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADER_FILES} ${SOURCE_FILES})

# ---- ライブラリを作成する ----

# ヘッダーファイルのみの場合、すべてのPUBLICフラグをINTERFACEに変更して、インターフェースターゲットを作成します
# target: add_library(${TARGET_NAME} INTERFACE)
add_library(${TARGET_NAME} ${HEADER_FILES} ${SOURCE_FILES})

set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 17)
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "Engine")

# クロスプラットフォームターゲットであるため、MSVCでの標準遵守を強制します
target_compile_options(${TARGET_NAME} PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/permissive->")
target_compile_options(${TARGET_NAME} PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/WX->")

# 依存関係をリンクする    
target_link_libraries(${TARGET_NAME} PUBLIC spdlog::spdlog)
target_link_libraries(${TARGET_NAME} PRIVATE tinyobjloader stb)
target_link_libraries(${TARGET_NAME} PUBLIC glfw)
target_link_libraries(${TARGET_NAME} PUBLIC imgui)
target_link_libraries(${TARGET_NAME} PUBLIC Jolt)
target_link_libraries(${TARGET_NAME} PUBLIC lua_static sol2)
target_link_libraries(${TARGET_NAME} PUBLIC ${vulkan_lib})
target_link_libraries(${TARGET_NAME} PRIVATE $<BUILD_INTERFACE:json11>)
target_link_libraries(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:json11>)
target_link_libraries(${TARGET_NAME} PUBLIC assimp)

if(ENABLE_PHYSICS_DEBUG_RENDERER)
  # 物理デバッグレンダリングを有効にする場合にコンパイル定義を追加
  add_compile_definitions(ENABLE_PHYSICS_DEBUG_RENDERER)
  # TestFrameworkとd3d12.lib、shcore.libをリンクする
  target_link_libraries(${TARGET_NAME} PUBLIC TestFramework d3d12.lib shcore.lib)
endif()

# ビルドインターフェースディレクトリにvulkan_includeを追加する
target_include_directories(
  ${TARGET_NAME}
  PUBLIC $<BUILD_INTERFACE:${vulkan_include}>)

# SHADER_COMPILE_TARGETの依存関係を追加し、ENGINE_ROOT_DIR/shader/generated/cppをインターフェースディレクトリに追加する
add_dependencies(${TARGET_NAME} ${SHADER_COMPILE_TARGET})
target_include_directories(
  ${TARGET_NAME}
  PUBLIC $<BUILD_INTERFACE:${ENGINE_ROOT_DIR}/shader/generated/cpp>)

# ビルドインターフェースディレクトリにENGINE_ROOT_DIR/sourceとinclude/TARGET_NAME-PROJECT_VERSIONを追加する
target_include_directories(
  ${TARGET_NAME} 
  PUBLIC $<BUILD_INTERFACE:${ENGINE_ROOT_DIR}/source>
  $<INSTALL_INTERFACE:include/${TARGET_NAME}-${PROJECT_VERSION}>
)

# ビルドインターフェースディレクトリにJSON_INCLUDEを追加する
target_include_directories(
  ${TARGET_NAME} 
  PUBLIC $<BUILD_INTERFACE:${JSON_INCLUDE}>
)

# ビルドインターフェースディレクトリにTHIRD_PARTY_DIR/vulkanmemoryallocator/includeを追加する
target_include_directories(
  ${TARGET_NAME} 
  PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/vulkanmemoryallocator/include>
)

# ビルドインターフェースディレクトリにENGINE_ROOT_DIR/source/runtimeとinclude/TARGET_NAME-PROJECT_VERSIONを追加する
target_include_directories(
  ${TARGET_NAME} 
  PUBLIC $<BUILD_INTERFACE:${ENGINE_ROOT_DIR}/source/runtime>
  $<INSTALL_INTERFACE:include/${TARGET_NAME}-${PROJECT_VERSION}>
)

# ビルドインターフェースディレクトリにENGINE_ROOT_DIR/source/runtime/function/render/includeとinclude/TARGET_NAME-PROJECT_VERSIONを追加する
target_include_directories(
  ${TARGET_NAME} 
  PUBLIC $<BUILD_INTERFACE:${ENGINE_ROOT_DIR}/source/runtime/function/render/include>
  $<INSTALL_INTERFACE:include/${TARGET_NAME}-${PROJECT_VERSION}>
)

# これにより、ユーザーは `find_package()` を使用してライブラリをインストールして検索できるようになります。

# プロジェクトのバージョンヘッダーが配置される場所は、プロジェクトの通常のヘッダーパスに一致する必要があります
string(TOLOWER ${TARGET_NAME}/version.h VERSION_HEADER_LOCATION)

### 事前コンパイル
# 事前コンパイルで使用されるグローバル変数を設定します
set(LunarYue_RUNTIME_HEADS "${HEADER_FILES}" PARENT_SCOPE)
