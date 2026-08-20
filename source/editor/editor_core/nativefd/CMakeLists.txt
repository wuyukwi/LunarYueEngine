file(GLOB_RECURSE libsrc CONFIGURE_DEPENDS *.h *.cpp *.hpp *.c *.cc)

add_library (nativefd ${libsrc})

target_link_libraries(nativefd PUBLIC filedialog)

set_target_properties(nativefd PROPERTIES FOLDER ${EDITOR_CORE_FOLDER})
