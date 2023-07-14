# ターゲット名を設定
set(TARGET_NAME ${SHADER_COMPILE_TARGET})

include(${THIRD_PARTY_DIR}/bgfx/cmake/bgfxToolUtils.cmake)

# 生成されたシェーダーファイルを保存するフォルダを作成
set(GENERATED_SHADER_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/generated)
if (NOT EXISTS ${GENERATED_SHADER_FOLDER})
  file(MAKE_DIRECTORY ${GENERATED_SHADER_FOLDER})
endif()

# シェーダーのインクルードフォルダを設定
set(SHADER_INCLUDE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/include)

function(add_shader_binary FILE)
get_filename_component(FILE_EXTENSION "${FILE}" EXT)
get_filename_component(FILENAME "${FILE}" NAME_WE)

if("${FILE_EXTENSION}" STREQUAL ".sc")
	string(SUBSTRING "${FILENAME}" 0 2 TYPE)
	if("${TYPE}" STREQUAL "fs")
		set(TYPE "FRAGMENT")
	elseif("${TYPE}" STREQUAL "vs")
		set(TYPE "VERTEX")
	elseif("${TYPE}" STREQUAL "cs")
		set(TYPE "COMPUTE")
	else()
		set(TYPE "")
	endif()
elseif("${FILE_EXTENSION}" STREQUAL ".fs")
    set(TYPE "FRAGMENT")
elseif("${FILE_EXTENSION}" STREQUAL ".vs")
    set(TYPE "VERTEX")
elseif("${FILE_EXTENSION}" STREQUAL ".cs")
    set(TYPE "COMPUTE")
else()
    set(TYPE "")
endif()

  if(NOT "${TYPE}" STREQUAL "")
		set(COMMON FILE ${FILE} ${TYPE} INCLUDES ${SHADER_INCLUDE_FOLDER})
		set(OUTPUTS "")
		set(OUTPUTS_PRETTY "")

		if(WIN32)
			# dx9
			if(NOT "${TYPE}" STREQUAL "COMPUTE")
				set(DX9_OUTPUT ${GENERATED_SHADER_FOLDER}/dx9/${FILENAME}.bin)
				_bgfx_shaderc_parse(
					DX9 ${COMMON} WINDOWS
					PROFILE s_3_0
					O 3
					OUTPUT ${DX9_OUTPUT}
				)
				list(APPEND OUTPUTS "DX9")
				set(OUTPUTS_PRETTY "${OUTPUTS_PRETTY}DX9, ")
			endif()

			# dx11
			set(DX11_OUTPUT ${GENERATED_SHADER_FOLDER}/dx11/${FILENAME}.bin)
			if(NOT "${TYPE}" STREQUAL "COMPUTE")
				_bgfx_shaderc_parse(
					DX11 ${COMMON} WINDOWS
					PROFILE s_5_0
					O 3
					OUTPUT ${DX11_OUTPUT}
				)
			else()
				_bgfx_shaderc_parse(
					DX11 ${COMMON} WINDOWS
					PROFILE s_5_0
					O 1
					OUTPUT ${DX11_OUTPUT}
				)
			endif()
			list(APPEND OUTPUTS "DX11")
			set(OUTPUTS_PRETTY "${OUTPUTS_PRETTY}DX11, ")
		endif()

		if(APPLE)
			# metal
			set(METAL_OUTPUT ${GENERATED_SHADER_FOLDER}/metal/${FILENAME}.bin)
			_bgfx_shaderc_parse(METAL ${COMMON} OSX PROFILE metal OUTPUT ${METAL_OUTPUT})
			list(APPEND OUTPUTS "METAL")
			set(OUTPUTS_PRETTY "${OUTPUTS_PRETTY}Metal, ")
		endif()

		# essl
		if(NOT "${TYPE}" STREQUAL "COMPUTE")
			set(ESSL_OUTPUT ${GENERATED_SHADER_FOLDER}/essl/${FILENAME}.bin)
			_bgfx_shaderc_parse(ESSL ${COMMON} ANDROID OUTPUT ${ESSL_OUTPUT})
			list(APPEND OUTPUTS "ESSL")
			set(OUTPUTS_PRETTY "${OUTPUTS_PRETTY}ESSL, ")
		endif()

		# glsl
		set(GLSL_OUTPUT ${GENERATED_SHADER_FOLDER}/glsl/${FILENAME}.bin)
		if(NOT "${TYPE}" STREQUAL "COMPUTE")
			_bgfx_shaderc_parse(GLSL ${COMMON} LINUX PROFILE 140 OUTPUT ${GLSL_OUTPUT})
		else()
			_bgfx_shaderc_parse(GLSL ${COMMON} LINUX PROFILE 430 OUTPUT ${GLSL_OUTPUT})
		endif()
		list(APPEND OUTPUTS "GLSL")
		set(OUTPUTS_PRETTY "${OUTPUTS_PRETTY}GLSL, ")

		# spirv
		if(NOT "${TYPE}" STREQUAL "COMPUTE")
			set(SPIRV_OUTPUT ${GENERATED_SHADER_FOLDER}/spirv/${FILENAME}.bin)
			_bgfx_shaderc_parse(SPIRV ${COMMON} LINUX PROFILE spirv OUTPUT ${SPIRV_OUTPUT})
			list(APPEND OUTPUTS "SPIRV")
			set(OUTPUTS_PRETTY "${OUTPUTS_PRETTY}SPIRV")
			set(OUTPUT_FILES "")
			set(COMMANDS "")
		endif()

		foreach(OUT ${OUTPUTS})
			list(APPEND OUTPUT_FILES ${${OUT}_OUTPUT})
			list(APPEND COMMANDS COMMAND "bgfx::shaderc" ${${OUT}})
			get_filename_component(OUT_DIR ${${OUT}_OUTPUT} DIRECTORY)
			file(MAKE_DIRECTORY ${OUT_DIR})
		endforeach()

		file(RELATIVE_PATH PRINT_NAME ${BGFX_DIR}/examples ${FILE})
		add_custom_command(
			MAIN_DEPENDENCY ${FILE} OUTPUT ${OUTPUT_FILES} ${COMMANDS}
			COMMENT "Compiling shader ${PRINT_NAME} for ${OUTPUTS_PRETTY}"
		)
  endif()
endfunction()

function(add_shader_headter FILE)
get_filename_component(FILE_EXTENSION "${FILE}" EXT)
get_filename_component(FILENAME "${FILE}" NAME_WE)

if("${FILE_EXTENSION}" STREQUAL ".sc")
	string(SUBSTRING "${FILENAME}" 0 2 TYPE)
	if("${TYPE}" STREQUAL "fs")
		set(TYPE "FRAGMENT")
	elseif("${TYPE}" STREQUAL "vs")
		set(TYPE "VERTEX")
	elseif("${TYPE}" STREQUAL "cs")
		set(TYPE "COMPUTE")
	else()
		set(TYPE "")
	endif()
elseif("${FILE_EXTENSION}" STREQUAL ".fs")
    set(TYPE "FRAGMENT")
elseif("${FILE_EXTENSION}" STREQUAL ".vs")
    set(TYPE "VERTEX")
elseif("${FILE_EXTENSION}" STREQUAL ".cs")
    set(TYPE "COMPUTE")
else()
    set(TYPE "")
endif()

  if(NOT "${TYPE}" STREQUAL "")
		set(COMMON FILE ${FILE} ${TYPE} INCLUDES ${SHADER_INCLUDE_FOLDER})
		set(OUTPUTS "")
		set(OUTPUTS_PRETTY "")

		if(WIN32)
			# dx9
			if(NOT "${TYPE}" STREQUAL "COMPUTE")
				set(DX9_OUTPUT ${GENERATED_SHADER_FOLDER}/dx9/${FILENAME}.bin)
				_bgfx_shaderc_parse(
					DX9 ${COMMON} WINDOWS
					PROFILE s_3_0
					O 3
					OUTPUT ${DX9_OUTPUT}
				)
				list(APPEND OUTPUTS "DX9")
				set(OUTPUTS_PRETTY "${OUTPUTS_PRETTY}DX9, ")
			endif()

			# dx11
			set(DX11_OUTPUT ${GENERATED_SHADER_FOLDER}/dx11/${FILENAME}.bin)
			if(NOT "${TYPE}" STREQUAL "COMPUTE")
				_bgfx_shaderc_parse(
					DX11 ${COMMON} WINDOWS
					PROFILE s_5_0
					O 3
					OUTPUT ${DX11_OUTPUT}
				)
			else()
				_bgfx_shaderc_parse(
					DX11 ${COMMON} WINDOWS
					PROFILE s_5_0
					O 1
					OUTPUT ${DX11_OUTPUT}
				)
			endif()
			list(APPEND OUTPUTS "DX11")
			set(OUTPUTS_PRETTY "${OUTPUTS_PRETTY}DX11, ")
		endif()

		if(APPLE)
			# metal
			set(METAL_OUTPUT ${GENERATED_SHADER_FOLDER}/metal/${FILENAME}.bin)
			_bgfx_shaderc_parse(METAL ${COMMON} OSX PROFILE metal OUTPUT ${METAL_OUTPUT})
			list(APPEND OUTPUTS "METAL")
			set(OUTPUTS_PRETTY "${OUTPUTS_PRETTY}Metal, ")
		endif()

		# essl
		if(NOT "${TYPE}" STREQUAL "COMPUTE")
			set(ESSL_OUTPUT ${GENERATED_SHADER_FOLDER}/essl/${FILENAME}.bin)
			_bgfx_shaderc_parse(ESSL ${COMMON} ANDROID OUTPUT ${ESSL_OUTPUT})
			list(APPEND OUTPUTS "ESSL")
			set(OUTPUTS_PRETTY "${OUTPUTS_PRETTY}ESSL, ")
		endif()

		# glsl
		set(GLSL_OUTPUT ${GENERATED_SHADER_FOLDER}/glsl/${FILENAME}.bin)
		if(NOT "${TYPE}" STREQUAL "COMPUTE")
			_bgfx_shaderc_parse(GLSL ${COMMON} LINUX PROFILE 140 OUTPUT ${GLSL_OUTPUT})
		else()
			_bgfx_shaderc_parse(GLSL ${COMMON} LINUX PROFILE 430 OUTPUT ${GLSL_OUTPUT})
		endif()
		list(APPEND OUTPUTS "GLSL")
		set(OUTPUTS_PRETTY "${OUTPUTS_PRETTY}GLSL, ")

		# spirv
		if(NOT "${TYPE}" STREQUAL "COMPUTE")
			set(SPIRV_OUTPUT ${GENERATED_SHADER_FOLDER}/spirv/${FILENAME}.bin)
			_bgfx_shaderc_parse(SPIRV ${COMMON} LINUX PROFILE spirv OUTPUT ${SPIRV_OUTPUT})
			list(APPEND OUTPUTS "SPIRV")
			set(OUTPUTS_PRETTY "${OUTPUTS_PRETTY}SPIRV")
			set(OUTPUT_FILES "")
			set(COMMANDS "")
		endif()

		foreach(OUT ${OUTPUTS})
			list(APPEND OUTPUT_FILES ${${OUT}_OUTPUT})
			list(APPEND COMMANDS COMMAND "bgfx::shaderc" ${${OUT}})
			get_filename_component(OUT_DIR ${${OUT}_OUTPUT} DIRECTORY)
			file(MAKE_DIRECTORY ${OUT_DIR})
		endforeach()

		file(RELATIVE_PATH PRINT_NAME ${BGFX_DIR}/examples ${FILE})
		add_custom_command(
			MAIN_DEPENDENCY ${FILE} OUTPUT ${OUTPUT_FILES} ${COMMANDS}
			COMMENT "Compiling shader ${PRINT_NAME} for ${OUTPUTS_PRETTY}"
		)
  endif()
endfunction()

function(auto_compile_shader_to_header SHADER_FILE)
get_filename_component(FILE_EXTENSION "${SHADER_FILE}" EXT)
get_filename_component(FILENAME "${SHADER_FILE}" NAME_WE)

if("${FILE_EXTENSION}" STREQUAL ".sc")
	string(SUBSTRING "${FILENAME}" 0 2 TYPE)
	if("${TYPE}" STREQUAL "fs")
		set(TYPE "FRAGMENT")
	elseif("${TYPE}" STREQUAL "vs")
		set(TYPE "VERTEX")
	elseif("${TYPE}" STREQUAL "cs")
		set(TYPE "COMPUTE")
	else()
		set(TYPE "")
	endif()
elseif("${FILE_EXTENSION}" STREQUAL ".fs")
    set(TYPE "FRAGMENT")
elseif("${FILE_EXTENSION}" STREQUAL ".vs")
    set(TYPE "VERTEX")
elseif("${FILE_EXTENSION}" STREQUAL ".cs")
    set(TYPE "COMPUTE")
else()
    set(TYPE "")
endif()

bgfx_compile_shader_to_header(
		TYPE ${TYPE}
		SHADERS ${SHADER_FILE}
		VARYING_DEF varying.def.sc
        OUTPUT_DIR ${SHADER_INCLUDE_FOLDER}/generated
)

endfunction()

set(SHADER_DIR_LIST
    "${CMAKE_CURRENT_SOURCE_DIR}/glsl"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

set(SOURCES "")
set(SHADERS "")

foreach(DIR ${SHADER_DIR_LIST})
	  	file(GLOB GLOB_SOURCES ${DIR}/*.h ${DIR}/*.fs ${DIR}/*.vs ${DIR}/*.cs ${DIR}/*.sc)
		list(APPEND SOURCES ${GLOB_SOURCES})
		file(GLOB GLOB_SHADERS ${DIR}/*.fs ${DIR}/*.vs ${DIR}/*.cs ${DIR}/*.sc)
		list(APPEND SHADERS ${GLOB_SHADERS})
endforeach()

if(NOT IOS
	   AND NOT EMSCRIPTEN
	   AND NOT ANDROID
)
		foreach(SHADER ${SHADERS})
			add_shader_binary(${SHADER})
			auto_compile_shader_to_header(${SHADER})
		endforeach()
		source_group("Shader Files" FILES ${SHADERS})
endif()


file(GLOB GENERATED_FILES ${GENERATED_SHADER_FOLDER}/*.bin)
file(GLOB GENERATED_HEADER ${SHADER_INCLUDE_FOLDER}/generated/*.h)

set(GENERATED_SHADER_FILE ${GENERATED_FILES})

add_custom_target(${TARGET_NAME} DEPENDS ${GENERATED_SHADER_FILE} ${SHADERS} ${GENERATED_HEADER} SOURCES ${SOURCES} ${GENERATED_HEADER})

add_custom_command(
    TARGET ${TARGET_NAME}
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/detect_new_shaders.cmake
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# ターゲットのプロパティを設定
set_target_properties("${TARGET_NAME}" PROPERTIES FOLDER "Engine" )
