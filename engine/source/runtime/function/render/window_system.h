#pragma once

#include <GLFW/glfw3.h>

#include <array>
#include <functional>
#include <vector>

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

    protected:
        // private:
        entry::WindowState m_state;

        uint32_t           m_width;
        uint32_t           m_height;
        uint32_t           m_debug;
        uint32_t           m_reset;
        entry::WindowState m_windows[8];
    };
} // namespace LunarYue
