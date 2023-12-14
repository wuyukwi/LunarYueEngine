file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_library (console ${libsrc})

include(target_warning_support)
set_warning_level(console ultra)
