file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_library (logging ${libsrc})

target_link_libraries(logging PUBLIC spdlog common_lib)

include(target_warning_support)
set_warning_level(logging ultra)
