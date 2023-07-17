add_subdirectory( filedialog )

target_include_directories (filedialog PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(filedialog PROPERTIES FOLDER ${THIRD_PARTY_FOLDER})
