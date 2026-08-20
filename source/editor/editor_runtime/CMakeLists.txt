set(EDITOR_RUNTIME_FOLDER ${EDITOR_FOLDER}/editor_runtime)

file(GLOB_RECURSE libsrc CONFIGURE_DEPENDS "*.h" "*.cpp" "*.hpp" "*.c" "*.cc")
file(GLOB EDITOR_RESOURCE ${CMAKE_CURRENT_SOURCE_DIR}/resource/LunarYueEditor.rc)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${libsrc} ${EDITOR_RESOURCE})

add_executable (${EDITOR_NAME} ${libsrc} ${EDITOR_RESOURCE})

target_link_libraries(${EDITOR_NAME} PUBLIC editor_core)
target_link_libraries(${EDITOR_NAME} PUBLIC runtime)

target_include_directories (${EDITOR_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)


if(MINGW)
	set_target_properties(${EDITOR_NAME} PROPERTIES LINK_FLAGS "-static-libgcc -static-libstdc++ -static")
endif()


check_cxx_compiler_flag("-Wa,-mbig-obj" HAS_MBIG_OBJ)

if(HAS_MBIG_OBJ)
    target_compile_options(${EDITOR_NAME} PUBLIC "-Wa,-mbig-obj")
elseif(MSVC)
    target_compile_options(${EDITOR_NAME} PUBLIC "/bigobj")

endif()

set(POST_BUILD_COMMANDS
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/${ASSET_DIR}" "$<TARGET_FILE_DIR:${EDITOR_NAME}>/${ASSET_DIR}"
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${BGFX_DIR}/src" "$<TARGET_FILE_DIR:${EDITOR_NAME}>/${ASSET_DIR}/src")

add_custom_command(TARGET ${EDITOR_NAME} POST_BUILD ${POST_BUILD_COMMANDS})


set_target_properties(${EDITOR_NAME} PROPERTIES FOLDER ${EDITOR_RUNTIME_FOLDER})

