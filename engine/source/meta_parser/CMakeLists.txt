# ビルド対象の名前を設定します
set(TARGET_NAME LunarYueParser)

# フォルダーを使用するようにグローバルプロパティを設定します
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# 全てのヘッダーファイルを検索し、リストアップします
file(GLOB_RECURSE HEADERS "*.h")

# 全てのソースファイルを検索し、リストアップします
file(GLOB_RECURSE SOURCES "*.cpp")

# ソースグループを設定します
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADERS} ${SOURCES})

# LLVMのインクルードディレクトリを設定します
set(LLVM_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/LLVM/include ${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/mustache ${CMAKE_CURRENT_SOURCE_DIR})

# リリースビルド時の出力先を設定します
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${ENGINE_ROOT_DIR}/bin)

# デバッグビルド時の出力先を設定します
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${ENGINE_ROOT_DIR}/bin)

# LLVMのインクルードディレクトリを追加します
include_directories(${LLVM_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/parser)

# ヘッダーファイルは必要ありませんが、一部のIDE（Visual Studio）では、
# ソースリストに追加されないため、追加する必要があります
add_executable(${TARGET_NAME} ${HEADERS} ${SOURCES})

# ターゲットプロパティを設定します
set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 17)
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "Tools")

if (CMAKE_HOST_WIN32) # Windowsの場合
    set(LLVM_LIBRARY_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/LLVM/lib/x64)
    set(LLVM_SHARED_LIBRARY_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/LLVM/bin/x64)
    set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} /O2 /Ob2")
    target_link_libraries(${TARGET_NAME}  ${LLVM_LIBRARY_DIR}/libclang.lib)
elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux") # Linuxの場合
    set(LLVM_LIBRARY_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/LLVM/lib/Linux)
    set(LLVM_SHARED_LIBRARY_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/LLVM/bin/Linux)
    set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O3")
    target_link_libraries(${TARGET_NAME}  ${LLVM_SHARED_LIBRARY_DIR}/libclang.so.12)
elseif(${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "arm64") # arm64の場合
    # XCode Toolchains' universal binary libclang.dylib にリンク
    set(LLVM_LIBRARY_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/LLVM/lib/macOS)
    set(LLVM_SHARED_LIBRARY_DIR  ${OSX_DEVELOPER_ROOT}/Toolchains/XcodeDefault.xctoolchain/usr/lib)
    set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O3")
    target_link_libraries(${TARGET_NAME}  ${LLVM_SHARED_LIBRARY_DIR}/libclang.dylib)    
else() # macOSの場合
    set(LLVM_LIBRARY_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/LLVM/lib/macOS)
    set(LLVM_SHARED_LIBRARY_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/LLVM/bin/macOS)
    set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O3")
    target_link_libraries(${TARGET_NAME}  ${LLVM_SHARED_LIBRARY_DIR}/libclang.dylib)
endif()



#BoostとLibClangを静的にリンクする
add_definitions(-DTIXML_USE_STL)

#ビルド後、リソースをコピーする
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
# mustache テンプレートディレクトリ
COMMAND ${CMAKE_COMMAND} -E copy_directory
"${LLVM_SHARED_LIBRARY_DIR}"
$<TARGET_FILE_DIR:${TARGET_NAME}>
)

