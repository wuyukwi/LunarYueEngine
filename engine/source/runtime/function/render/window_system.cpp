#include "runtime/function/render/window_system.h"

#include "runtime/core/base/macro.h"

#include <GLFW/glfw3.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/bx.h>
#include <stdio.h>
#if BX_PLATFORM_LINUX
#define GLFW_EXPOSE_NATIVE_X11
#elif BX_PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#elif BX_PLATFORM_OSX
#define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include <GLFW/glfw3native.h>

namespace LunarYue
{
    static void error_callback(int _error, const char* _description) { LOG_ERROR("GLFW error %d: %s", _error, _description); }

    WindowSystem::~WindowSystem()
    {
        glfwDestroyWindowImpl(m_window);
        glfwTerminate();
    }

    void WindowSystem::initialize(WindowCreateInfo create_info)
    {
        glfwSetErrorCallback(error_callback);

        if (!glfwInit())
        {
            LOG_FATAL(__FUNCTION__, "failed to initialize GLFW");
            return;
        }

        m_width  = create_info.width;
        m_height = create_info.height;

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        m_window = glfwCreateWindow(create_info.width, create_info.height, create_info.title, nullptr, nullptr);
        if (!m_window)
        {
            LOG_FATAL(__FUNCTION__, "failed to create window");
            glfwTerminate();
            return;
        }

        // Setup input callbacks
        glfwSetWindowUserPointer(m_window, this);
        glfwSetKeyCallback(m_window, keyCallback);
        glfwSetCharCallback(m_window, charCallback);
        glfwSetCharModsCallback(m_window, charModsCallback);
        glfwSetMouseButtonCallback(m_window, mouseButtonCallback);
        glfwSetCursorPosCallback(m_window, cursorPosCallback);
        glfwSetCursorEnterCallback(m_window, cursorEnterCallback);
        glfwSetScrollCallback(m_window, scrollCallback);
        glfwSetDropCallback(m_window, dropCallback);
        glfwSetWindowSizeCallback(m_window, windowSizeCallback);
        glfwSetWindowCloseCallback(m_window, windowCloseCallback);

        glfwSetInputMode(m_window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
    }

    void WindowSystem::pollEvents() const { glfwPollEvents(); }

    void WindowSystem::setShouldClose(bool value) const { glfwSetWindowShouldClose(m_window, value); }

    bool WindowSystem::shouldClose() const { return glfwWindowShouldClose(m_window); }

    void WindowSystem::setTitle(const char* title) { glfwSetWindowTitle(m_window, title); }

    void* WindowSystem::getWindow() const { return glfwNativeWindowHandle(m_window); }

    std::array<int, 2> WindowSystem::getWindowSize() const { return {m_width, m_height}; }

    void WindowSystem::setFocusMode(bool mode)
    {
        m_is_focus_mode = mode;
        glfwSetInputMode(m_window, GLFW_CURSOR, m_is_focus_mode ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }

    void* WindowSystem::glfwNativeWindowHandle(GLFWwindow* _window) const
    {
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
#if ENTRY_CONFIG_USE_WAYLAND
        wl_egl_window* win_impl = (wl_egl_window*)glfwGetWindowUserPointer(_window);
        if (!win_impl)
        {
            int width, height;
            glfwGetWindowSize(_window, &width, &height);
            struct wl_surface* surface = (struct wl_surface*)glfwGetWaylandWindow(_window);
            if (!surface)
                return nullptr;
            win_impl = wl_egl_window_create(surface, width, height);
            glfwSetWindowUserPointer(_window, (void*)(uintptr_t)win_impl);
        }
        return (void*)(uintptr_t)win_impl;
#else
        return (void*)(uintptr_t)glfwGetX11Window(_window);
#endif
#elif BX_PLATFORM_OSX
        return glfwGetCocoaWindow(_window);
#elif BX_PLATFORM_WINDOWS
        return glfwGetWin32Window(_window);
#endif // BX_PLATFORM_
    }

    void WindowSystem::glfwDestroyWindowImpl(GLFWwindow* _window)
    {
        if (!_window)
            return;
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
#if ENTRY_CONFIG_USE_WAYLAND
        wl_egl_window* win_impl = (wl_egl_window*)glfwGetWindowUserPointer(_window);
        if (win_impl)
        {
            glfwSetWindowUserPointer(_window, nullptr);
            wl_egl_window_destroy(win_impl);
        }
#endif
#endif
        glfwDestroyWindow(_window);
    }

} // namespace LunarYue
