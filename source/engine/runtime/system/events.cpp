#include "events.h"

namespace runtime
{
    hpp::event<void(float)> on_frame_begin;
    hpp::event<void(float)> on_frame_update;
    hpp::event<void(float)> on_frame_render;
    hpp::event<void(float)> on_frame_ui_render;
    hpp::event<void(float)> on_frame_end;

    hpp::event<void(const std::vector<SDL_Event>&)> on_platform_events;
} // namespace runtime
