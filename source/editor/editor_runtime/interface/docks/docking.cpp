#include "docking.h"

docking_system::docking_system() { runtime::on_platform_events.connect(this, &docking_system::platform_events); }

docking_system::~docking_system() { runtime::on_platform_events.disconnect(this, &docking_system::platform_events); }

void docking_system::platform_events(const std::vector<SDL_Event>& events)
{
    for (const auto& e : events)
    {
        if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE)
        {
            / dockspaces_.erase(e.window.windowID);
            return;
        }
    }
}
