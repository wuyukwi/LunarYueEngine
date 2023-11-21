////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "render_window.h"

void render_window::on_resize() { render_window::prepare_surface(); }

render_window::render_window() : window_sdl() { render_window::prepare_surface(); }

render_window::render_window(const char* title, int32_t w, int32_t h, int32_t x, int32_t y, std::uint32_t flags, uint32_t id) :
    window_sdl(title, w, h, x, y, flags)
{

    render_window::prepare_surface();
}

render_window::~render_window()
{
    window_sdl::~window_sdl();

    render_window::destroy_surface();
}

std::shared_ptr<gfx::frame_buffer> render_window::get_surface() const { return surface_; }

void render_window::destroy_surface()
{
    // force internal handle destruction
    if (surface_)
    {
        surface_->dispose();
        surface_.reset();

        gfx::flush();
    }
}

gfx::view_id render_window::begin_present_pass()
{
    const gfx::render_pass pass("present_to_window_pass");
    pass.bind(surface_.get());
    pass.clear();

    return pass.id;
}

void render_window::prepare_surface()
{
    const auto size = get_size();

    surface_ = std::make_shared<gfx::frame_buffer>(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(get_native_window_handle())),
                                                   static_cast<std::uint16_t>(size[0]),
                                                   static_cast<std::uint16_t>(size[1]));
}
