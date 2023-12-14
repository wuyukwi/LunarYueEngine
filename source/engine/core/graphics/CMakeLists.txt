file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_library (graphics ${libsrc})

target_link_libraries(graphics PUBLIC bgfx bx)
target_link_libraries(graphics PRIVATE example-common)
target_compile_definitions( graphics PRIVATE "MAX_RENDER_PASSES=${MAX_VIEWS}" )

include(target_warning_support)
set_warning_level(graphics high)
