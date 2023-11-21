#include "docking.h"

docking_system::docking_system() { runtime::on_platform_events.connect(this, &docking_system::platform_events); }

docking_system::~docking_system()
{
    runtime::on_platform_events.disconnect(this, &docking_system::platform_events);

    docks_.clear();
    dockspaces_.clear();
}

imguidock::dockspace& docking_system::get_dockspace(uint32_t id)
{
    const auto it = dockspaces_.find(id);
    if (it == dockspaces_.end())
    {
        imguidock::dockspace dockspace;
        dockspace.owner_id = id;
        dockspaces_.emplace(id, std::move(dockspace));
    }

    return dockspaces_[id];
}

const std::map<uint32_t, imguidock::dockspace>& docking_system::get_dockspaces() const { return dockspaces_; }

void docking_system::register_dock(std::unique_ptr<imguidock::dock> dock) { docks_.emplace_back(std::move(dock)); }

void docking_system::platform_events(const const std::vector<SDL_Event>& events)
{
    for (const auto& e : events)
    {
        if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE)
        {
            dockspaces_.erase(e.window.windowID);
            return;
        }
    }
}
