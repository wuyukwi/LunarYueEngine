#pragma once

#include <runtime/system/events.h>

#include <map>
#include <memory>
#include <vector>

struct docking_system
{
    docking_system();
    ~docking_system();

private:
    void platform_events(const std::vector<SDL_Event>& events);
};
