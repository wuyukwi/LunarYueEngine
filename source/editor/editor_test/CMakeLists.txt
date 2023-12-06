set(EDITOR_TEST_FOLDER ${EDITOR_FOLDER}/editor_test)
set(EDITOR_TEST_NAME test_editor)

set(libsrc
    main.cpp
    imgui_impl_dx11.cpp
    imgui_impl_sdl2.cpp
)

set(libheaders
    imgui_impl_sdl2.h
    imgui_impl_dx11.h
)

set(EDITOR_RESOURCE
)

add_executable(${EDITOR_TEST_NAME} ${libsrc} ${libheaders} ${EDITOR_RESOURCE})
#file(GLOB_RECURSE libsrc "*.h" "*.cpp" "*.hpp" "*.c" "*.cc")
#add_executable (${EDITOR_TEST_NAME} ${libsrc} ${EDITOR_RESOURCE})

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${libsrc} ${EDITOR_RESOURCE})

set_target_properties(${EDITOR_TEST_NAME} PROPERTIES FOLDER ${EDITOR_TEST_FOLDER})

target_link_libraries(${EDITOR_TEST_NAME} PUBLIC editor_core)
target_link_libraries(${EDITOR_TEST_NAME} PUBLIC runtime)
target_link_libraries(${EDITOR_TEST_NAME} PUBLIC d3d11)

target_include_directories (${EDITOR_TEST_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

