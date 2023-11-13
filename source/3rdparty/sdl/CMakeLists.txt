# Create an option to switch between a system sdl library and a vendored SDL library
option(MYGAME_VENDORED "Use vendored libraries" ON)

set(SDL_VIDEO_ENABLED_BY_DEFAULT ON)
set(SDL_RENDER_ENABLED_BY_DEFAULT OFF)
set(SDL_AUDIO_ENABLED_BY_DEFAULT OFF)
set(SDL_JOYSTICK_ENABLED_BY_DEFAULT ON)
set(SDL_HAPTIC_ENABLED_BY_DEFAULT OFF)
set(SDL_POWER_ENABLED_BY_DEFAULT OFF)
set(SDL_TIMERS_ENABLED_BY_DEFAULT OFF)
set(SDL_FILE_ENABLED_BY_DEFAULT OFF)
set(SDL_LOADSO_ENABLED_BY_DEFAULT ON)
set(SDL_CPUINFO_ENABLED_BY_DEFAULT OFF)
set(SDL_FILESYSTEM_ENABLED_BY_DEFAULT OFF)
set(SDL_SENSOR_ENABLED_BY_DEFAULT OFF)
set(SDL_LOCALE_ENABLED_BY_DEFAULT OFF)
set(SDL_MISC_ENABLED_BY_DEFAULT OFF)

if(MYGAME_VENDORED)
    add_subdirectory(SDL EXCLUDE_FROM_ALL)
else()
    # 1. Look for a SDL2 package, 2. look for the SDL2 component and 3. fail if none can be found
    find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)

    # 1. Look for a SDL2 package, 2. Look for the SDL2maincomponent and 3. DO NOT fail when SDL2main is not available
    find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)
endif()

if(TARGET SDL2::SDL2main)
	set_target_properties(SDL2main PROPERTIES FOLDER ${THIRD_PARTY_FOLDER}/SDL)
endif()

set_target_properties(SDL2-static PROPERTIES FOLDER ${THIRD_PARTY_FOLDER}/SDL)
set_target_properties(sdl_headers_copy PROPERTIES FOLDER ${THIRD_PARTY_FOLDER}/SDL)