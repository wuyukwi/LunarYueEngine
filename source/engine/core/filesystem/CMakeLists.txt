file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.c *.cc)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${libsrc})

add_library (filesystem ${libsrc})

set_target_properties(filesystem PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

include(target_warning_support)
set_warning_level(filesystem high)
