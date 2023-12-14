# Create an option to switch between a system sdl library and a vendored SDL library
option(MYGAME_VENDORED "Use vendored libraries" ON)

set(SDL_ATOMIC OFF)
set(SDL_AUDIO OFF)
set(SDL_VIDEO ON)
set(SDL_RENDER OFF)
set(SDL_EVENTS ON)
set(SDL_JOYSTICK ON)
set(SDL_HAPTIC ON)
set(SDL_HIDAPI ON)
set(SDL_POWER OFF)
set(SDL_TIMERS ON)
set(SDL_FILE OFF)
set(SDL_CPUINFO OFF)
set(SDL_FILESYSTEM OFF)
set(SDL_SENSOR ON)
set(SDL_LOCALE ON)
set(SDL_MISC ON)

if(MYGAME_VENDORED)
    add_subdirectory(sdl3 EXCLUDE_FROM_ALL)
else()
    find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-static)

endif()

set_target_properties(SDL3-static PROPERTIES FOLDER ${THIRD_PARTY_FOLDER}/SDL)
#set_target_properties(Headers PROPERTIES FOLDER ${THIRD_PARTY_FOLDER}/SDL)