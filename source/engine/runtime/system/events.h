#pragma once

#include <core/common_lib/basetypes.hpp>
#include <core/signals/event.hpp>

#include "SDL2/SDL_events.h"

#include <cstdint>

namespace runtime
{
    /// engine loop events
    extern hpp::event<void(float)> on_frame_begin;
    extern hpp::event<void(float)> on_frame_update;
    extern hpp::event<void(float)> on_frame_render;
    extern hpp::event<void(float)> on_frame_ui_render;
    extern hpp::event<void(float)> on_frame_end;

    /// platform events

    ///{events}
    extern hpp::event<void(const std::vector<SDL_Event>&)> on_platform_events;

} // namespace runtime
