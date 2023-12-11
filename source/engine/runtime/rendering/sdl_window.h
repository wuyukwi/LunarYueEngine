#pragma once

#include <array>
#include <memory>

struct ImGuiPlatformIO;
class SDL_Window;
struct SDL_Cursor;

class sdl_window
{
public:
    sdl_window(const char* title, int32_t w, int32_t h, int32_t x, int32_t y, std::uint32_t flags);
    ~sdl_window();

    void set_visible(bool visible);
    bool is_visible() const;

    void maximize();
    void minimize();
    void restore();

    void set_title(const char* title);
    void set_mouse_cursor(SDL_Cursor* cursor);
    void show_cursor(bool visible);

    void raise_window();
    bool has_focus();

    void* get_native_window_handle();
    void  destroy_window();

    std::array<std::int32_t, 2> get_position() const;

    void set_position(const std::array<std::int32_t, 2>& position);

    std::array<std::uint32_t, 2> get_size() const;
    std::array<std::uint32_t, 2> get_drawable_size() const;

    std::array<int, 2> get_mouse_position_global();
    std::array<int, 2> get_mouse_position_in_window();

    void set_mouse_position_global(const std::array<int, 2>& position);
    void set_mouse_position_in_window(const std::array<int, 2>& position);

    void set_mouse_cursor_visible(bool visible);
    void set_mouse_cursor_grabbed(bool grabbed);

    void set_opacity(float opacity);

    bool window_is_minimized();

    uint32_t get_sdl_window_id();

    void request_close();

    virtual void set_size(const std::array<std::uint32_t, 2>& size);

private:
    SDL_Window* window_ = nullptr;
};
