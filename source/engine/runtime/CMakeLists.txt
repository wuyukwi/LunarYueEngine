set(ENGINE_RUNTIME_FOLDER ${ENGINE_FOLDER}/runtime)

file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${libsrc})

add_library (runtime ${libsrc})

set_target_properties(runtime PROPERTIES FOLDER ${ENGINE_RUNTIME_FOLDER})

target_link_libraries(runtime PUBLIC core)
# SDL2::SDL2main may or may not be available. It is e.g. required by Windows GUI applications
if(TARGET SDL2::SDL2main)
    # It has an implicit dependency on SDL2 functions, so it MUST be added before SDL2::SDL2 (or SDL2::SDL2-static)
    target_link_libraries(runtime PUBLIC SDL2main)
endif()

# Link to the actual SDL2 library. SDL2::SDL2 is the shared SDL library, SDL2::SDL2-static is the static SDL libarary.
target_link_libraries(runtime PUBLIC SDL2-static)

target_include_directories (runtime PUBLIC ${ENGINE_ROOT_DIR})


