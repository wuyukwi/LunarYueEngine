####################################################################################################
# This function converts any file into C/C++ source code.
# Example:
# - input file: data.dat
# - output file: data.h
# - variable name declared in output file: DATA
# - data length: sizeof(DATA)
# embed_resource("data.dat" "data.h" "DATA")
####################################################################################################

function(embed_resource resource_file_name source_file_name variable_name)

    if(EXISTS "${source_file_name}")
	# 2つのファイルを比較し、最初のファイルが2番目のファイルより新しい（つまり、最終変更日時がより最近である）場合に真（True）を返し、それ以外の場合は偽（False）を返します。
        if("${source_file_name}" IS_NEWER_THAN "${resource_file_name}")
            return()
        endif()
    endif()

    if(EXISTS "${resource_file_name}")
	# 指定されたファイルからデータを読み込みます。読み込んだデータは、hex_contentに格納されます。HEXオプションは、ファイルの内容を16進数形式で読み込むことを指定します。
        file(READ "${resource_file_name}" hex_content HEX)

		# 32文字の16進数のパターンを作成します
        string(REPEAT "[0-9a-f]" 32 pattern)
		
		# リソースファイルのデータ（hex_content）を行ごとに分割し、content変数に代入します。
        string(REGEX REPLACE "(${pattern})" "\\1\n" content "${hex_content}")

		# content内のすべての16進数のバイト値の前に0xを付け、後ろに（カンマ）を付けます。これにより、C++の配列形式に変換されます。
        string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1, " content "${content}")

		# 最後のカンマを取り除きます。
        string(REGEX REPLACE ", $" "" content "${content}")
		
		# array_definition変数にC++の配列定義を設定します。ここで、std::vector<unsigned char>としてデータを格納しています。
        set(array_definition "static const std::vector<unsigned char> ${variable_name} =\n{\n${content}\n};")
        
		# 出力ファイルの名前を取得します。
        get_filename_component(file_name ${source_file_name} NAME)
		# 最終的なソースコードを生成し、source変数に格納します。このソースコードには、ファイル名、説明コメント、#include <vector>、および配列定義が含まれます。
        set(source "/**\n * @file ${file_name}\n * @brief Auto generated file.\n */\n#include <vector>\n${array_definition}\n")

		# 生成されたソースコードを指定された出力ファイルに書き込みます。
        file(WRITE "${source_file_name}" "${source}")
    else()
        message("ERROR: ${resource_file_name} doesn't exist")
        return()
    endif()

endfunction()

# CMakeスクリプトがスタンドアロンのスクリプトとして使用された場合
if(EXISTS "${PATH}")
	# リソースファイル（${PATH}）を、指定されたヘッダーファイル（${HEADER}）に埋め込みます。この際、埋め込まれるデータの変数名は${GLOBAL}となります。
    embed_resource("${PATH}" "${HEADER}" "${GLOBAL}")
endif() 
