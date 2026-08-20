file(GLOB_RECURSE libsrc CONFIGURE_DEPENDS *.h *.cpp *.hpp *.c *.cc)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${libsrc})

add_library (gui ${libsrc})

target_link_libraries(gui PUBLIC imgui)

set_target_properties(gui PROPERTIES FOLDER ${EDITOR_CORE_FOLDER})
