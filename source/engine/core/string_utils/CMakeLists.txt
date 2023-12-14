file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_library (string_utils ${libsrc})

include(target_warning_support)
set_warning_level(string_utils ultra)
