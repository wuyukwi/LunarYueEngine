file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_library (mesh_import ${libsrc})

target_link_libraries(mesh_import PUBLIC assimp)

set_target_properties(mesh_import PROPERTIES FOLDER ${EDITOR_CORE_FOLDER})
