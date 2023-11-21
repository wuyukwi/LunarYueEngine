#include "events.h"

namespace runtime
{
    hpp::event<void(delta_t)> on_frame_begin;
    hpp::event<void(delta_t)> on_frame_update;
    hpp::event<void(delta_t)> on_frame_render;
    hpp::event<void(delta_t)> on_frame_ui_render;
    hpp::event<void(delta_t)> on_frame_end;

    hpp::event<void(const std::vector<SDL_Event>&)> on_platform_events;
} // namespace runtime
