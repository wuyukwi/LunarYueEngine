file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

add_library (simulation ${libsrc})

include(target_warning_support)
set_warning_level(simulation ultra)
