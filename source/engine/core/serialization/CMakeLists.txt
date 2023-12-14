file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${libsrc})

add_library (serialization ${libsrc})

target_link_libraries(serialization PUBLIC cereal)

include(target_warning_support)
set_warning_level(serialization ultra)
