set( SPDLOG_BUILD_EXAMPLES OFF CACHE BOOL "Build examples" FORCE)
set( SPDLOG_BUILD_TESTING OFF CACHE BOOL "Build spdlog tests" FORCE)

add_subdirectory( spdlog )

set_target_properties(spdlog PROPERTIES FOLDER ${THIRD_PARTY_FOLDER})
