# 获取当前文件夹中的所有shader文件
file(GLOB CURRENT_SHADERS "*.fs" "*.vs" "*.cs" "*.sc")

foreach(CURRENT_SHADER ${CURRENT_SHADERS})
    # 提取文件名部分
    get_filename_component(CURRENT_SHADER_NAME ${CURRENT_SHADER} NAME)

    # 构建已编译shader的路径
    set(COMPILED_SHADER_PATH "${GENERATED_SHADER_FOLDER}/${CURRENT_SHADER_NAME}.bin")

    # 检查当前文件是否已经编译过
    list(FIND COMPILED_SHADERS "${COMPILED_SHADER_PATH}" INDEX)
    if(INDEX EQUAL -1)
        # 如果文件没有被编译过，则调用add_shader函数进行编译
        add_shader_binary(${CURRENT_SHADER})
        auto_compile_shader_to_header(${CURRENT_SHADER})
    endif()
endforeach()
