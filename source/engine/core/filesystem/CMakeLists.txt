file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${libsrc})

add_library (filesystem ${libsrc})

include(target_warning_support)
set_warning_level(filesystem high)
