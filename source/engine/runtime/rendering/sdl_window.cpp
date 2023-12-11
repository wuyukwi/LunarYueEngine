#include "sdl_window.h"

#include "core/graphics/graphics.h"
#include "core/logging/logging.h"

#include "SDL2/SDL.h"

BX_PRAGMA_DIAGNOSTIC_PUSH()
BX_PRAGMA_DIAGNOSTIC_IGNORED_CLANG("-Wextern-c-compat")
#include "SDL2/SDL_syswm.h"
BX_PRAGMA_DIAGNOSTIC_POP()

sdl_window::sdl_window(const char* title, int32_t w, int32_t h, int32_t x, int32_t y, std::uint32_t flags)
{
    if (x == -1)
        x = SDL_WINDOWPOS_CENTERED;
    if (y == -1)
        y = SDL_WINDOWPOS_CENTERED;
    if (flags == 0)
        flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;

    window_ = (SDL_CreateWindow(title, x, y, w, h, flags));
}

sdl_window::~sdl_window() { SDL_DestroyWindow(window_); }

void sdl_window::set_visible(bool visible)
{
    if (visible)
        SDL_ShowWindow(window_);
    else
        SDL_HideWindow(window_);
}

bool sdl_window::is_visible() const
{
    Uint32 windowFlags = SDL_GetWindowFlags(window_);

    bool isVisible = (windowFlags & SDL_WINDOW_SHOWN) != 0;

    return isVisible;
}

void sdl_window::raise_window() { SDL_RaiseWindow(window_); }

void sdl_window::maximize() { SDL_MaximizeWindow(window_); }

void sdl_window::minimize() { SDL_MinimizeWindow(window_); }

void sdl_window::restore() { SDL_RestoreWindow(window_); }

void sdl_window::set_title(const char* title) { SDL_SetWindowTitle(window_, title); }

void sdl_window::set_mouse_cursor(SDL_Cursor* cursor) { SDL_SetCursor(cursor); }
void sdl_window::show_cursor(bool visible)
{
    if (visible)
        SDL_ShowCursor(SDL_TRUE);
    else
        SDL_ShowCursor(SDL_FALSE);
};

bool sdl_window::has_focus()
{
    Uint32 windowFlags = SDL_GetWindowFlags(window_);

    return (windowFlags & SDL_WINDOW_INPUT_FOCUS) != 0;
}

void* sdl_window::get_native_window_handle()
{
    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);
    if (!SDL_GetWindowWMInfo(window_, &wmi))
    {
        APPLOG_ERROR("SDL_GetWindowWMInfo failed: {}", SDL_GetError());

        SDL_ClearError();

        return nullptr;
    }

#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
#if ENTRY_CONFIG_USE_WAYLAND
    wl_egl_window* win_impl = (wl_egl_window*)SDL_GetWindowData(window_, "wl_egl_window");
    if (!win_impl)
    {
        int width, height;
        SDL_GetWindowSize(window_, &width, &height);
        struct wl_surface* surface = wmi.info.wl.surface;
        if (!surface)
            return nullptr;
        win_impl = wl_egl_window_create(surface, width, height);
        SDL_SetWindowData(window_, "wl_egl_window", win_impl);
    }
    return (void*)(uintptr_t)win_impl;
#else
    return (void*)wmi.info.x11.window;
#endif
#elif BX_PLATFORM_OSX || BX_PLATFORM_IOS
    return wmi.info.cocoa.window;
#elif BX_PLATFORM_WINDOWS
    return wmi.info.win.window;
#elif BX_PLATFORM_ANDROID
    return wmi.info.android.window;
#endif // BX_PLATFORM_
}

void sdl_window::destroy_window()
{
    if (!window_)
        return;
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
#if ENTRY_CONFIG_USE_WAYLAND
    wl_egl_window* win_impl = (wl_egl_window*)SDL_GetWindowData(window_, "wl_egl_window");
    if (win_impl)
    {
        SDL_SetWindowData(window_, "wl_egl_window", nullptr);
        wl_egl_window_destroy(win_impl);
    }
#endif
#endif
    SDL_DestroyWindow(window_);
}

std::array<std::int32_t, 2> sdl_window::get_position() const
{
    std::array<std::int32_t, 2> position = {0, 0};
    SDL_GetWindowPosition(window_, &position[0], &position[1]);
    return position;
}

void sdl_window::set_position(const std::array<std::int32_t, 2>& position) { SDL_SetWindowPosition(window_, position[0], position[1]); }

std::array<std::uint32_t, 2> sdl_window::get_size() const
{
    int w, h;
    SDL_GetWindowSize(window_, &w, &h);

    return {static_cast<std::uint32_t>(w), static_cast<std::uint32_t>(h)};
}

std::array<std::uint32_t, 2> sdl_window::get_drawable_size() const
{
    int w, h;
    SDL_GL_GetDrawableSize(window_, &w, &h);

    return {static_cast<std::uint32_t>(w), static_cast<std::uint32_t>(h)};
}

void sdl_window::set_size(const std::array<std::uint32_t, 2>& size) { SDL_SetWindowSize(window_, size[0], size[1]); }

std::array<int, 2> sdl_window::get_mouse_position_global()
{
    std::array<int, 2> position;
    SDL_GetMouseState(&position[0], &position[1]);
    return position;
}

std::array<int, 2> sdl_window::get_mouse_position_in_window()
{
    std::array<int, 2> position;
    SDL_GetMouseState(&position[0], &position[1]);

    int windowX, windowY;
    SDL_GetWindowPosition(window_, &windowX, &windowY);

    position[0] -= windowX;
    position[1] -= windowY;

    return position;
}

void sdl_window::set_mouse_position_global(const std::array<int, 2>& position) { SDL_WarpMouseGlobal(position[0], position[1]); }

void sdl_window::set_mouse_position_in_window(const std::array<int, 2>& position)
{
    int windowX, windowY;
    SDL_GetWindowPosition(window_, &windowX, &windowY);

    SDL_WarpMouseInWindow(window_, position[0] + windowX, position[1] + windowY);
}

void sdl_window::set_mouse_cursor_visible(bool visible) { SDL_ShowCursor(visible ? SDL_ENABLE : SDL_DISABLE); }

void sdl_window::set_mouse_cursor_grabbed(bool grabbed) { SDL_SetRelativeMouseMode(grabbed ? SDL_TRUE : SDL_FALSE); }

void sdl_window::set_opacity(float opacity)
{
    // Ensure opacity is within the valid range [0.0, 1.0]
    opacity = std::max(0.0f, std::min(1.0f, opacity));

    // Convert opacity to Uint8 (0-255)
    Uint8 alpha = static_cast<Uint8>(opacity * 255);

    // Set window opacity
    SDL_SetWindowOpacity(window_, alpha);
}

bool sdl_window::window_is_minimized() { return (SDL_GetWindowFlags(window_) & SDL_WINDOW_MINIMIZED) != 0; }

uint32_t sdl_window::get_sdl_window_id() { return SDL_GetWindowID(window_); }

void sdl_window::request_close()
{
    // Create an SDL event for closing the window
    SDL_Event event;
    std::memset(&event, 0, sizeof(event));

    event.type            = SDL_WINDOWEVENT;
    event.window.event    = SDL_WINDOWEVENT_CLOSE;
    event.window.windowID = SDL_GetWindowID(window_);

    // Push the event to the event queue
    SDL_PushEvent(&event);
}
