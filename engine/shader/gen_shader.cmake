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