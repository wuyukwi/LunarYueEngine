set(EDITOR_RUNTIME_FOLDER ${EDITOR_FOLDER}/runtime)
set(TARGET_NAME "editor")

file(GLOB_RECURSE libsrc "*.h" "*.cpp" "*.hpp" "*.c" "*.cc")

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${libsrc})

add_executable (${TARGET_NAME} ${libsrc})

target_link_libraries(${TARGET_NAME} PUBLIC editor_core)
target_link_libraries(${TARGET_NAME} PUBLIC runtime)

target_include_directories (${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)


if(MINGW)
	set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS "-static-libgcc -static-libstdc++ -static")
endif()


check_cxx_compiler_flag("-Wa,-mbig-obj" HAS_MBIG_OBJ)

if(HAS_MBIG_OBJ)
    target_compile_options(${TARGET_NAME} PUBLIC "-Wa,-mbig-obj")
elseif(MSVC)
    target_compile_options(${TARGET_NAME} PUBLIC "/bigobj")

endif()

set(POST_BUILD_COMMANDS
#  COMMAND ${CMAKE_COMMAND} -E make_directory "${BINARY_ROOT_DIR}"   
#  COMMAND ${CMAKE_COMMAND} -E copy_directory "$<TARGET_FILE_DIR:${TARGET_NAME}>/" "${BINARY_ROOT_DIR}"  
#  COMMAND ${CMAKE_COMMAND} -E remove_directory "${BINARY_ROOT_DIR}/${ASSET_DIR}" 
#  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/${ASSET_DIR}" "${BINARY_ROOT_DIR}/${ASSET_DIR}"
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/${ASSET_DIR}" "$<TARGET_FILE_DIR:${TARGET_NAME}>/${ASSET_DIR}" 
#  COMMAND ${CMAKE_COMMAND} -E copy_directory "${BGFX_DIR}/src" "${BINARY_ROOT_DIR}/src"
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${BGFX_DIR}/src" "$<TARGET_FILE_DIR:${TARGET_NAME}>/scr"
)

add_custom_command(TARGET ${TARGET_NAME} ${POST_BUILD_COMMANDS})


set_target_properties(${TARGET_NAME} PROPERTIES FOLDER ${EDITOR_RUNTIME_FOLDER})

