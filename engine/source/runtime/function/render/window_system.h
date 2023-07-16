#pragma once

#include <functional>
#include <map>
#include <string>

#include "bgfx/defines.h"
#include "entry/entry.h"

namespace LunarYue
{
    struct WindowInfo
    {
        uint32_t m_width {1280};
        uint32_t m_height {720};
        uint32_t m_debug {BGFX_DEBUG_TEXT};
        uint32_t m_reset {BGFX_RESET_VSYNC};
    };

    class WindowSystem
    {
    public:
        WindowSystem() = default;
        ~WindowSystem();

        void               initialize(WindowInfo create_info); // Initialize the window system
        bool               processWindowEvents();              // Poll events
        void               setTitle(const char* title);        // Set the window title
        WindowInfo         getWindowInfo() const;              // Get the window size
        entry::WindowState getWindowState() const;
        void*              getWindowHandle();
        void*              getDisplayHandle();
        void               createWindow(const std::string& name);
        void               destroyWindow(const std::string& name);

    protected:
        entry::WindowState m_state;

        uint32_t m_width;
        uint32_t m_height;
        uint32_t m_debug;
        uint32_t m_reset;

        std::map<std::string, entry::WindowState> m_windows;
    };
} // namespace LunarYue
