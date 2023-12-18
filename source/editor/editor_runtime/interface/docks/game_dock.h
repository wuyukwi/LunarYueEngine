#pragma once
#include <string>
class console_log;
struct game_dock
{
    game_dock(const std::string& dtitle);

    void render();
};
