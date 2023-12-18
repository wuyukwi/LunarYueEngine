#pragma once

#include <memory>
#include <string>

class console_log;
struct console_dock
{
    console_dock(const std::string& title, const std::shared_ptr<console_log>& log);

    void render();

private:
    std::shared_ptr<console_log> console_log_;
};
