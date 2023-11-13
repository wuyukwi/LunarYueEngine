#pragma once

#include <core/common_lib/basetypes.hpp>
#include <core/signals/event.hpp>

#include "SDL2/SDL_events.h"

#include <chrono>
#include <cstdint>

namespace runtime
{
    /// engine loop events
    extern hpp::event<void(delta_t)> on_frame_begin;
    extern hpp::event<void(delta_t)> on_frame_update;
    extern hpp::event<void(delta_t)> on_frame_render;
    extern hpp::event<void(delta_t)> on_frame_ui_render;
    extern hpp::event<void(delta_t)> on_frame_end;

    /// platform events

    /// <window_id, is_focused>, {events}
    extern hpp::event<void(const std::pair<std::uint32_t, bool>&, const std::vector<SDL_Event>&)> on_platform_events;

} // namespace runtime
