file(GLOB_RECURSE libsrc *.h *.cpp *.hpp *.inl *.c *.cc *.m)

add_library (stb_vorbis ${libsrc})
target_include_directories (stb_vorbis PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(stb_vorbis PROPERTIES FOLDER ${THIRD_PARTY_FOLDER})
