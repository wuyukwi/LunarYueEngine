# 
set(PRECOMPILE_TOOLS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(LunarYue_PRECOMPILE_PARAMS_IN_PATH "${CMAKE_CURRENT_SOURCE_DIR}/source/precompile/precompile.json.in")
set(LunarYue_PRECOMPILE_PARAMS_PATH "${PRECOMPILE_TOOLS_PATH}/precompile.json")
configure_file(${LunarYue_PRECOMPILE_PARAMS_IN_PATH} ${LunarYue_PRECOMPILE_PARAMS_PATH})

#
# use wine for linux
if (CMAKE_HOST_WIN32)
    set(PRECOMPILE_PRE_EXE)
	set(PRECOMPILE_PARSER ${PRECOMPILE_TOOLS_PATH}/LunarYueParser.exe)
    set(sys_include "*") 
elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux" )
    set(PRECOMPILE_PRE_EXE)
	set(PRECOMPILE_PARSER ${PRECOMPILE_TOOLS_PATH}/LunarYueParser)
    set(sys_include "/usr/include/c++/9/") 
    #execute_process(COMMAND chmod a+x ${PRECOMPILE_PARSER} WORKING_DIRECTORY ${PRECOMPILE_TOOLS_PATH})
elseif(CMAKE_HOST_APPLE)
    find_program(XCRUN_EXECUTABLE xcrun)
    if(NOT XCRUN_EXECUTABLE)
      message(FATAL_ERROR "xcrun not found!!!")
    endif()

    execute_process(
      COMMAND ${XCRUN_EXECUTABLE} --sdk macosx --show-sdk-platform-path
      OUTPUT_VARIABLE osx_sdk_platform_path_test
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    set(PRECOMPILE_PRE_EXE)
	set(PRECOMPILE_PARSER ${PRECOMPILE_TOOLS_PATH}/LunarYueParser)
    set(sys_include "${osx_sdk_platform_path_test}/../../Toolchains/XcodeDefault.xctoolchain/usr/include/c++/v1") 
endif()

set (PARSER_INPUT ${CMAKE_BINARY_DIR}/parser_header.h)
set(PRECOMPILE_TARGET "LunarYuePreCompile")

### BUILDING ====================================================================================

add_custom_command(OUTPUT ${PARSER_INPUT}
                   COMMAND ${PRECOMPILE_PARSER} "${LunarYue_PRECOMPILE_PARAMS_PATH}" "${PARSER_INPUT}" "${ENGINE_ROOT_DIR}/source" ${sys_include} "LunarYue" 0
                   DEPENDS ${LunarYue_PRECOMPILE_PARAMS_PATH}
                   COMMENT "Precompiling"
                   VERBATIM)

add_custom_target(${PRECOMPILE_TARGET} ALL
                  DEPENDS ${PARSER_INPUT}
                  COMMENT "Precompile finished")

add_dependencies(${PRECOMPILE_TARGET} LunarYuePreCompile)
