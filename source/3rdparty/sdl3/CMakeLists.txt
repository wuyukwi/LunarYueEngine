add_subdirectory(SDL EXCLUDE_FROM_ALL)

set_target_properties(SDL3-static PROPERTIES FOLDER ${THIRD_PARTY_FOLDER}/SDL3)
set_target_properties(SDL_uclibc PROPERTIES FOLDER ${THIRD_PARTY_FOLDER}/SDL3)

