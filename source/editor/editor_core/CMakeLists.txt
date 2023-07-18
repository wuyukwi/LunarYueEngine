set(EDITOR_CORE_FOLDER ${EDITOR_FOLDER}/editor_core)

add_subdirectory(mesh_import)
add_subdirectory(nativefd)
add_subdirectory(gui)

add_library (editor_core core.cpp)


target_link_libraries(editor_core PUBLIC mesh_import)
target_link_libraries(editor_core PUBLIC nativefd)
target_link_libraries(editor_core PUBLIC gui)

target_include_directories (editor_core PUBLIC ${EDITOR_FOLDER})

set_target_properties(editor_core PROPERTIES FOLDER ${EDITOR_CORE_FOLDER})

