set(ENGINE_CORE_FOLDER ${ENGINE_FOLDER}/core)

add_library (core INTERFACE)
target_include_directories (core INTERFACE ${ENGINE_ROOT_DIR})

function(add_subdirectory_and_link_libraries DIRECTORY LIBRARIES)
    add_subdirectory(${DIRECTORY})
    target_link_libraries(core INTERFACE ${LIBRARIES})
    set_target_properties(${LIBRARIES} PROPERTIES FOLDER ${ENGINE_FOLDER}/core)
endfunction()

set(DIRECTORIES
    common_lib
    cmd_line
    config
    console
    filesystem
    audio
    graphics
    logging
    math
    memory
    reflection
    serialization
    signals
    simulation
    system
    string_utils
    tasks
    uuid
)

foreach(DIRECTORY ${DIRECTORIES})
    add_subdirectory_and_link_libraries(${DIRECTORY} ${DIRECTORY})
endforeach()

