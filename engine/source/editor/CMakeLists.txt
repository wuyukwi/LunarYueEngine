# 変数の設定
set(TARGET_NAME LunarYueEditor)

# ファイルの検索
file(GLOB EDITOR_HEADERS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
file(GLOB EDITOR_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)
file(GLOB EDITOR_RESOURCE CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/resource/LunarYueEditor.rc)

# ソースグループの作成
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${EDITOR_HEADERS} ${EDITOR_SOURCES} ${EDITOR_RESOURCE})

# 実行ファイルの作成
add_executable(${TARGET_NAME} ${EDITOR_HEADERS} ${EDITOR_SOURCES} ${EDITOR_RESOURCE})

# 定義の追加
add_compile_definitions("LunarYue_ROOT_DIR=${BINARY_ROOT_DIR}")
add_compile_definitions("LunarYueEditorConfigFile=\"${TARGET_NAME}.ini\"")

# インクルードディレクトリの設定
target_include_directories(
  ${TARGET_NAME}
  PUBLIC $<BUILD_INTERFACE:${THIRD_PARTY_DIR}/stb>
)

# ターゲットプロパティの設定
set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 17 OUTPUT_NAME "LunarYueEditor")
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "Engine")

# コンパイルオプションの設定
target_compile_options(${TARGET_NAME} PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/WX->")

# ライブラリのリンク設定
target_link_libraries(${TARGET_NAME} LunarYueRuntime)
#target_link_libraries(${TARGET_NAME} PUBLIC bgfx)
#target_link_libraries(${TARGET_NAME} PUBLIC example-common)

# POST_BUILD_COMMANDSには、プロジェクトがビルドされた後に実行されるカスタムコマンドが含まれます
set(POST_BUILD_COMMANDS
  COMMAND ${CMAKE_COMMAND} -E make_directory "${BINARY_ROOT_DIR}"  # バイナリのルートディレクトリがない場合は作成する
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/resource" "${BINARY_ROOT_DIR}/resource"  # リソースフォルダをバイナリのルートディレクトリにコピーする
  COMMAND ${CMAKE_COMMAND} -E copy_directory "$<TARGET_FILE_DIR:${TARGET_NAME}>/" "${BINARY_ROOT_DIR}"  # 実行可能ファイルをバイナリのルートディレクトリにコピーする
  COMMAND ${CMAKE_COMMAND} -E copy "${ENGINE_ROOT_DIR}/${DEPLOY_CONFIG_DIR}/${TARGET_NAME}.ini" "${BINARY_ROOT_DIR}"  # デプロイ設定ファイルをバイナリのルートディレクトリにコピーする
  COMMAND ${CMAKE_COMMAND} -E copy "${ENGINE_ROOT_DIR}/${DEVELOP_CONFIG_DIR}/${TARGET_NAME}.ini" "$<TARGET_FILE_DIR:${TARGET_NAME}>/"  # 開発設定ファイルを実行可能ファイルと同じ場所にコピーする
  COMMAND ${CMAKE_COMMAND} -E remove_directory "${BINARY_ROOT_DIR}/${ENGINE_ASSET_DIR}"  # エンジンアセットフォルダがあれば削除する
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${ENGINE_ROOT_DIR}/${ENGINE_ASSET_DIR}" "${BINARY_ROOT_DIR}/${ENGINE_ASSET_DIR}"  # エンジンアセットフォルダをバイナリのルートディレクトリにコピーする
)

# 物理デバッグレンダラが有効な場合、Joltアセットフォルダもバイナリのルートディレクトリにコピーする
if(ENABLE_PHYSICS_DEBUG_RENDERER)
  set(POST_BUILD_COMMANDS ${POST_BUILD_COMMANDS}
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${BINARY_ROOT_DIR}/${JOLT_ASSET_DIR}"  # Joltアセットフォルダがあれば削除する
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${ENGINE_ROOT_DIR}/${JOLT_ASSET_DIR}" "${BINARY_ROOT_DIR}/${JOLT_ASSET_DIR}"  # Joltアセットフォルダをバイナリのルートディレクトリにコピーする
)
endif()

# POST_BUILD_COMMANDSをターゲットに追加する
add_custom_command(TARGET ${TARGET_NAME} ${POST_BUILD_COMMANDS})

# precompile
# これは、precompileで使用されるグローバル変数を設定します。
set(LunarYue_EDITOR_HEADS "${EDITOR_HEADERS}" PARENT_SCOPE)  # プリコンパイルで使用されるLunarYue_EDITOR_HEADS変数をグローバルスコープに

