#pragma once
#include <string>

struct scene_dock
{
    scene_dock(const std::string& dtitle);

    void render();
    // void show_statistics(const ImVec2& area, unsigned int fps, bool& show_gbuffer);
    bool show_gbuffer    = false;
    bool enable_profiler = false;
};
