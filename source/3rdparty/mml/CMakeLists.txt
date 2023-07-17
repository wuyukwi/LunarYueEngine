add_subdirectory( mml )

target_include_directories (mml-window PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/mml/include)

set_target_properties(mml-system PROPERTIES FOLDER ${THIRD_PARTY_FOLDER}/mml)
set_target_properties(mml-window PROPERTIES FOLDER ${THIRD_PARTY_FOLDER}/mml)
