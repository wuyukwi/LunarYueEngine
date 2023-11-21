#pragma once

#include <array>
#include <memory>

#include "SDL2/SDL.h"

struct ImGuiPlatformIO;

class window_sdl
{
public:
    window_sdl();
    window_sdl(const char* title, int32_t w, int32_t h, int32_t x, int32_t y, std::uint32_t flags);
    ~window_sdl();

    void set_visible(bool visible);
    bool is_visible() const;

    void maximize();
    void minimize();
    void restore();

    void set_title(const char* title);
    void set_mouse_cursor(SDL_Cursor* cursor);

    void raise_window();
    bool has_focus();

    void* get_native_window_handle();
    void  destroy_window();

    std::array<std::int32_t, 2> get_position() const;

    void set_position(const std::array<std::int32_t, 2>& position);

    std::array<std::uint32_t, 2> get_size() const;

    void set_size(const std::array<std::uint32_t, 2>& size);

    std::array<int, 2> get_mouse_position_global();
    std::array<int, 2> get_mouse_position_in_window();

    void set_mouse_position_global(const std::array<int, 2>& position);
    void set_mouse_position_in_window(const std::array<int, 2>& position);

    void set_mouse_cursor_visible(bool visible);
    void set_mouse_cursor_grabbed(bool grabbed);

    void set_opacity(float opacity);

    bool get_window_minimized();

    uint32_t get_window_id();

    void request_close();

    static bool poll_event(SDL_Event& event);

private:
    SDL_Window* window_;
};
