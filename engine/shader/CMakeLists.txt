# ターゲット名を設定
set(TARGET_NAME LunarYueShaderCompile)

include(${THIRD_PARTY_DIR}/bgfx/cmake/bgfxToolUtils.cmake)
#include(${CMAKE_CURRENT_SOURCE_DIR}/gen_shader.cmake)
function(add_bgfx_shader FILE OUTPUT_DIR INCLUDE_DIRS)
	get_filename_component(FILENAME "${FILE}" NAME_WE)
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

	if(NOT "${TYPE}" STREQUAL "")
		set(COMMON FILE ${FILE} ${TYPE} INCLUDES ${INCLUDE_DIRS})
		set(OUTPUTS "")
		set(OUTPUTS_PRETTY "")

		if(WIN32)
			# dx9
			if(NOT "${TYPE}" STREQUAL "COMPUTE")
				set(DX9_OUTPUT ${OUTPUT_DIR}/shaders/dx9/${FILENAME}.bin)
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
			set(DX11_OUTPUT ${OUTPUT_DIR}/shaders/dx11/${FILENAME}.bin)
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
			set(METAL_OUTPUT ${OUTPUT_DIR}/shaders/metal/${FILENAME}.bin)
			_bgfx_shaderc_parse(METAL ${COMMON} OSX PROFILE metal OUTPUT ${METAL_OUTPUT})
			list(APPEND OUTPUTS "METAL")
			set(OUTPUTS_PRETTY "${OUTPUTS_PRETTY}Metal, ")
		endif()

		# essl
		if(NOT "${TYPE}" STREQUAL "COMPUTE")
			set(ESSL_OUTPUT ${OUTPUT_DIR}/shaders/essl/${FILENAME}.bin)
			_bgfx_shaderc_parse(ESSL ${COMMON} ANDROID OUTPUT ${ESSL_OUTPUT})
			list(APPEND OUTPUTS "ESSL")
			set(OUTPUTS_PRETTY "${OUTPUTS_PRETTY}ESSL, ")
		endif()

		# glsl
		set(GLSL_OUTPUT ${OUTPUT_DIR}/shaders/glsl/${FILENAME}.bin)
		if(NOT "${TYPE}" STREQUAL "COMPUTE")
			_bgfx_shaderc_parse(GLSL ${COMMON} LINUX PROFILE 140 OUTPUT ${GLSL_OUTPUT})
		else()
			_bgfx_shaderc_parse(GLSL ${COMMON} LINUX PROFILE 430 OUTPUT ${GLSL_OUTPUT})
		endif()
		list(APPEND OUTPUTS "GLSL")
		set(OUTPUTS_PRETTY "${OUTPUTS_PRETTY}GLSL, ")

		# spirv
		if(NOT "${TYPE}" STREQUAL "COMPUTE")
			set(SPIRV_OUTPUT ${OUTPUT_DIR}/shaders/spirv/${FILENAME}.bin)
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

		add_custom_command(
			MAIN_DEPENDENCY ${FILE} OUTPUT ${OUTPUT_FILES} ${COMMANDS}
			COMMENT "Compiling shader for ${OUTPUTS_PRETTY}"
		)
	endif()
endfunction()

# 生成されたシェーダーファイルを保存するフォルダを作成
set(GENERATED_SHADER_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/generated")

# シェーダーのインクルードフォルダを設定
set(SHADER_SCR_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/scr)
set(VARYING_DEF ${CMAKE_CURRENT_SOURCE_DIR}/glsl/varying.def.sc)

set(SHADER_DIR_LIST
    "${CMAKE_CURRENT_SOURCE_DIR}/glsl"
    "${CMAKE_CURRENT_SOURCE_DIR}/scr"
)

set(SOURCES "")
set(VERTEX_SHADERS "")
set(FRAGMENT_SHADERS "")
set(SHADERS "")

foreach(DIR ${SHADER_DIR_LIST})
	file(GLOB GLOB_SOURCES ${DIR}/*.h ${DIR}/*.cpp ${DIR}/*.sh)
	list(APPEND SOURCES ${GLOB_SOURCES})

	file(GLOB GLOB_VERTEX_SHADERS ${DIR}/*.vs ${DIR}/*.vert)
	list(APPEND VERTEX_SHADERS ${GLOB_VERTEX_SHADERS})
	file(GLOB GLOB_FRAGMENT_SHADERS ${DIR}/*.fs ${DIR}/*.frag)
	list(APPEND FRAGMENT_SHADERS ${GLOB_FRAGMENT_SHADERS})

	file(GLOB GLOB_BGFX_SHADERS ${DIR}/*.sc)
	foreach(BGFX_SHADER ${GLOB_BGFX_SHADERS})
		get_filename_component(FILENAME ${BGFX_SHADER} NAME_WE)
		string(SUBSTRING "${FILENAME}" 0 2 TYPE)
		if("${TYPE}" STREQUAL "fs")
			list(APPEND FRAGMENT_SHADERS ${BGFX_SHADER})
		elseif("${TYPE}" STREQUAL "vs")
			list(APPEND VERTEX_SHADERS ${BGFX_SHADER})
		elseif("${TYPE}" STREQUAL "cs")
		else()
	endif()
	endforeach()

	list(APPEND SHADERS ${VERTEX_SHADERS} ${FRAGMENT_SHADERS})
endforeach()

function(GEN_SHADER_HEADER_FILES FLODER FILE_NAME)
		set(COMBINED_SHADER_FILE ${FLODER}/${FILE_NAME}.gen.h)
		file(WRITE ${COMBINED_SHADER_FILE} "")
		file(GLOB SHADER_HEADER_FILES ${FLODER}/${FILE_NAME}/*.h)
		foreach(FILE ${SHADER_HEADER_FILES})
			file(READ ${FILE} CONTENTS)
			file(APPEND ${COMBINED_SHADER_FILE} "${CONTENTS}")
		endforeach()
endfunction()

set(GENERATED_HEADERS "")
if(NOT IOS AND NOT ANDROID)
	foreach(SHADER ${VERTEX_SHADERS})
		get_filename_component(SHADER_NAME ${SHADER} NAME_WE)
		bgfx_compile_shader_to_header(
			TYPE VERTEX
			SHADERS ${SHADER}
  			VARYING_DEF ${VARYING_DEF}
  			OUTPUT_DIR ${GENERATED_SHADER_FOLDER}/vertex_shaders/${SHADER_NAME}
			OUT_FILES_VAR ${GENERATED_HEADERS}
			INCLUDE_DIRS ${SHADER_SCR_FOLDER}
		)
	endforeach()

	foreach(SHADER ${FRAGMENT_SHADERS})
		get_filename_component(SHADER_NAME ${SHADER} NAME_WE)
		bgfx_compile_shader_to_header(
			TYPE FRAGMENT
			SHADERS ${SHADER}
  			VARYING_DEF ${VARYING_DEF}
  			OUTPUT_DIR ${GENERATED_SHADER_FOLDER}/fragment_shaders/${SHADER_NAME}
			OUT_FILES_VAR
			INCLUDE_DIRS ${SHADER_SCR_FOLDER}
			)
	endforeach()

	foreach(SHADER ${SHADERS})
		add_bgfx_shader(SHADER ${GENERATED_SHADER_FOLDER} ${SHADER_SCR_FOLDER})
	endforeach()

endif()

source_group("Shader Files" FILES ${SHADERS})
source_group("VaryingDef" FILES ${VARYING_DEF})
source_group("Source Files" FILES ${SOURCES})

add_library(${TARGET_NAME}  ${SOURCES} ${SHADERS} ${VARYING_DEF})

target_include_directories(${TARGET_NAME} PUBLIC ${SHADER_SCR_FOLDER} ${GENERATED_SHADER_FOLDER})

set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "Engine" )

