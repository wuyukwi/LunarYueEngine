file(GLOB_RECURSE HEADER_FILES *.h *.hpp)
file(GLOB_RECURSE SOURCE_FILES *.cpp *.c *.cc)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HEADER_FILES} ${SOURCE_FILES})

add_library (audio ${HEADER_FILES} ${SOURCE_FILES})

target_link_libraries(audio PUBLIC OpenAL)
target_link_libraries(audio PUBLIC stb_vorbis)

include(target_warning_support)
set_warning_level(audio ultra)
