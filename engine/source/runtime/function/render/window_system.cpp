#include "runtime/function/render/window_system.h"

#include "runtime/core/base/macro.h"

namespace LunarYue
{
    WindowSystem::~WindowSystem() {}

    void WindowSystem::initialize(WindowInfo create_info)
    {
        m_width  = create_info.m_width;
        m_height = create_info.m_height;
        m_debug  = create_info.m_debug;
        m_reset  = create_info.m_reset;
    }

    bool WindowSystem::processWindowEvents() { return entry::processWindowEvents(m_state, m_debug, m_reset); }

    void WindowSystem::setTitle(const char* title) {}

    WindowInfo WindowSystem::getWindowInfo() const { return {m_width, m_height, m_debug, m_reset}; }

    entry::WindowState WindowSystem::getWindowState() const { return m_state; }
} // namespace LunarYue
