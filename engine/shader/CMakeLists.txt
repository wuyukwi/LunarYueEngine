# ターゲット名を設定
set(TARGET_NAME ${SHADER_COMPILE_TARGET})

# シェーダーファイルのパスを取得
file(GLOB_RECURSE SHADER_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/glsl/*.vert"
                                            "${CMAKE_CURRENT_SOURCE_DIR}/glsl/*.frag"
                                            "${CMAKE_CURRENT_SOURCE_DIR}/glsl/*.comp"
                                            "${CMAKE_CURRENT_SOURCE_DIR}/glsl/*.geom"
                                            "${CMAKE_CURRENT_SOURCE_DIR}/glsl/*.tesc"
                                            "${CMAKE_CURRENT_SOURCE_DIR}/glsl/*.tese"
                                            "${CMAKE_CURRENT_SOURCE_DIR}/glsl/*.mesh"
                                            "${CMAKE_CURRENT_SOURCE_DIR}/glsl/*.task"
                                            "${CMAKE_CURRENT_SOURCE_DIR}/glsl/*.rgen"
                                            "${CMAKE_CURRENT_SOURCE_DIR}/glsl/*.rchit"
                                            "${CMAKE_CURRENT_SOURCE_DIR}/glsl/*.rmiss"
                                            "${CMAKE_CURRENT_SOURCE_DIR}/glsl/*.rcall")

file(GLOB_RECURSE INCLUDE_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*")
SOURCE_GROUP("include" FILES ${INCLUDE_FILES})

# シェーダーファイルをグループ化
SOURCE_GROUP("shaders" FILES ${SHADER_FILES})

# シェーダーのインクルードフォルダを設定
set(SHADER_INCLUDE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/include)

# 生成されたシェーダーファイルを保存するフォルダを作成
set(GENERATED_SHADER_FOLDER "generated")
if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${GENERATED_SHADER_FOLDER})
  file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${GENERATED_SHADER_FOLDER})
endif()

# SPIR-V バイナリを保存するフォルダを作成
if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${GENERATED_SHADER_FOLDER}/spv)
  file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${GENERATED_SHADER_FOLDER}/spv)
endif()

# ShaderCompile.cmake ファイルを読み込み、シェーダーのコンパイルを実行
include(${LunarYue_ROOT_DIR}/cmake/ShaderCompile.cmake)
compile_shader(
  "${SHADER_FILES}"
  "${TARGET_NAME}"
  "${SHADER_INCLUDE_FOLDER}"
  "${INCLUDE_FILES}"
  "${GENERATED_SHADER_FOLDER}"
  "${glslangValidator_executable}")

# ターゲットのプロパティを設定
set_target_properties("${TARGET_NAME}" PROPERTIES FOLDER "Engine" )
