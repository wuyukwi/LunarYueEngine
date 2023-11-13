#pragma once

#include <array>
#include <memory>

#include "SDL2/SDL.h"

class window_sdl
{
public:
    window_sdl();
    window_sdl(const char* title, int w, int h, std::uint32_t flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
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
    bool poll_event(SDL_Event& event);

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

    void request_close();

private:
    SDL_Window* window_;
};
