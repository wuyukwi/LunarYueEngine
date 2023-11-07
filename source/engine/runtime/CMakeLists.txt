set(ENGINE_RUNTIME_FOLDER ${ENGINE_FOLDER}/runtime)

file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${libsrc})

add_library (runtime ${libsrc})

set_target_properties(runtime PROPERTIES FOLDER ${ENGINE_RUNTIME_FOLDER})

target_link_libraries(runtime PUBLIC core)
target_link_libraries(runtime PUBLIC mml-window)
target_link_libraries(runtime PUBLIC SDL3-static)

target_include_directories (runtime PUBLIC ${ENGINE_ROOT_DIR})


