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

    void* WindowSystem::getWindowHandle() { return entry::getNativeWindowHandle(entry::kDefaultWindowHandle); }

    void* WindowSystem::getDisplayHandle() { return entry::getNativeDisplayHandle(); }

    void WindowSystem::createWindow(const std::string& name)
    {
        entry::WindowHandle handle = entry::createWindow(rand() % 1280, rand() % 720, 640, 480);
        if (entry::isValid(handle))
        {
            LOG_INFO("createWindow {}", name)
            entry::setWindowTitle(handle, name.c_str());

            m_windows[name].m_handle = handle;
        }
    }

    void WindowSystem::destroyWindow(const std::string& name)
    {
        auto iter = m_windows.find(name);

        if (iter != m_windows.end())
        {
            entry::destroyWindow(iter->second.m_handle);
            m_windows.erase(iter);
        }
        else
            LOG_FATAL("destroyWindow name no find")
    }

} // namespace LunarYue
