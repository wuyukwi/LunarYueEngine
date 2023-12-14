file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_library (system ${libsrc})

target_link_libraries(system PUBLIC common_lib)

include(target_warning_support)
set_warning_level(system ultra)
