#pragma once
#include <runtime/ecs/ecs.h>

#include <string>
struct hierarchy_dock
{
    hierarchy_dock(const std::string& dtitle);

    void render();

    void draw_entity(runtime::entity entity);

private:
    bool     edit_label_ = false;
    uint32_t id_         = 0;
};
