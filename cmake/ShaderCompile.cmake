# このCMake関数compile_shaderは、GLSL（OpenGL Shading Language）シェーダファイルをコンパイルし、生成されたSPV（SPIR-V）ファイルとC++ヘッダファイルに変換します。

# この関数の目的は、GLSLシェーダファイルをコンパイルしてSPIR-V形式に変換し、それらをC++プログラムで利用できるようにヘッダーファイルに埋め込むことです。
# 関数は、シェーダのリストに対してループ処理を行い、それぞれのシェーダファイルをコンパイルし、生成されたバイナリデータをC++ヘッダーファイルに格納します。これにより、C++アプリケーション内でシェーダを簡単に使用できるようになります。

# 関数の引数：
# SHADERS: コンパイルするGLSLシェーダファイルのリスト。
# TARGET_NAME: 生成されるC++ヘッダファイルとSPVファイルの依存関係を表すカスタムターゲットの名前。
# SHADER_INCLUDE_FOLDER: シェーダファイルのインクルードパス。
# GENERATED_DIR: 生成されるSPVファイルとC++ヘッダファイルを格納するディレクトリ。
# GLSLANG_BIN: glslangValidatorバイナリへのパス。これはGLSLファイルをSPVファイルにコンパイルするために使用されます。

function(compile_shader SHADERS TARGET_NAME SHADER_INCLUDE_FOLDER GENERATED_DIR GLSLANG_BIN)

    set(working_dir "${CMAKE_CURRENT_SOURCE_DIR}")

    set(ALL_GENERATED_SPV_FILES "")
    set(ALL_GENERATED_CPP_FILES "")
	
	# UNIX環境であれば、glslangValidatorバイナリに実行権限を与えます。
    if(UNIX)
        execute_process(COMMAND chmod a+x ${GLSLANG_BIN})
    endif()

    foreach(SHADER ${SHADERS})
    # このシェーダーのためのヘッダ名とグローバル変数を準備する
	
        get_filename_component(SHADER_NAME ${SHADER} NAME)
		# SHADER_NAME（シェーダーファイル名）からピリオド（.）をアンダースコア（_）に置換して、新しい文字列HEADER_NAMEを生成します。これにより、ファイル名から拡張子を削除し、ヘッダーファイル名を作成します。
        string(REPLACE "." "_" HEADER_NAME ${SHADER_NAME})
		# HEADER_NAMEを大文字に変換して、GLOBAL_SHADER_VARという新しい文字列を生成します。これは、ヘッダーファイル内でシェーダーデータを格納するグローバル変数の名前として使用されます。
        string(TOUPPER ${HEADER_NAME} GLOBAL_SHADER_VAR)

		# SPV_FILEという変数に、生成されたSPIR-Vファイル（.spv）のパスを設定します。このパスは、現在のソースディレクトリ（CMAKE_CURRENT_SOURCE_DIR）、指定された生成ディレクトリ
        set(SPV_FILE "${CMAKE_CURRENT_SOURCE_DIR}/${GENERATED_DIR}/spv/${SHADER_NAME}.spv")
		# CPP_FILEという変数に、生成されるC++ヘッダーファイル（.h）のパスを設定します。このパスは、現在のソースディレクトリ（CMAKE_CURRENT_SOURCE_DIR）、指定された生成ディレクトリ（GENERATED_DIR）、cppサブディレクトリ、およびヘッダー名を使って構築されます。
        set(CPP_FILE "${CMAKE_CURRENT_SOURCE_DIR}/${GENERATED_DIR}/cpp/${HEADER_NAME}.h")

		# 指定されたシェーダーファイルをコンパイルしてSPVファイル（シェーダーのバイナリフォーマット）を生成するためのコマンドを追加しています。
		# このコマンドでは、GLSLANG_BIN（glslangValidatorのバイナリ）を使用してシェーダーをコンパイルします。
		# -I${SHADER_INCLUDE_FOLDER}オプションでインクルードフォルダーを指定し、-V100オプションでVulkan 1.00バージョンを指定し、-o ${SPV_FILE}オプションで出力ファイル名（SPVファイル）を指定しています。
        add_custom_command(
            OUTPUT ${SPV_FILE}
            COMMAND ${GLSLANG_BIN} -I${SHADER_INCLUDE_FOLDER} -V100 -o ${SPV_FILE} ${SHADER}
            DEPENDS ${SHADER}
            WORKING_DIRECTORY "${working_dir}")

        list(APPEND ALL_GENERATED_SPV_FILES ${SPV_FILE})
		
		# 生成されたSPVファイルを、C++ヘッダーファイルに埋め込むためのコマンドを追加しています。
		# このコマンドでは、CMakeを実行してGenerateShaderCPPFile.cmakeスクリプトを呼び出し、
		# -DPATH=${SPV_FILE}オプションでSPVファイルのパス、-DHEADER="${CPP_FILE}"オプションで出力ヘッダーファイル名、-DGLOBAL="${GLOBAL_SHADER_VAR}"オプションでグローバル変数名を指定しています。
        add_custom_command(
            OUTPUT ${CPP_FILE}
            COMMAND ${CMAKE_COMMAND} -DPATH=${SPV_FILE} -DHEADER="${CPP_FILE}" 
                -DGLOBAL="${GLOBAL_SHADER_VAR}" -P "${LunarYue_ROOT_DIR}/cmake/GenerateShaderCPPFile.cmake"
            DEPENDS ${SPV_FILE}
            WORKING_DIRECTORY "${working_dir}")

        list(APPEND ALL_GENERATED_CPP_FILES ${CPP_FILE})

    endforeach()

    add_custom_target(${TARGET_NAME}
        DEPENDS ${ALL_GENERATED_SPV_FILES} ${ALL_GENERATED_CPP_FILES} SOURCES ${SHADERS})

endfunction()