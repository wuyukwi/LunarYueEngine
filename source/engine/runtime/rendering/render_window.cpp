////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "render_window.h"

static std::uint32_t s_next_id = 0;

void render_window::on_resize() { render_window::prepare_surface(); }

render_window::render_window() : window_sdl()
{
    id_ = s_next_id++;
    render_window::prepare_surface();
}

render_window::render_window(const char* title, int w, int h, std::uint32_t flags) : window_sdl(title, w, h, flags)
{
    id_ = s_next_id++;
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

uint32_t render_window::get_id() const { return id_; }

void render_window::prepare_surface()
{
    const auto size = get_size();

    surface_ = std::make_shared<gfx::frame_buffer>(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(get_native_window_handle())),
                                                   static_cast<std::uint16_t>(size[0]),
                                                   static_cast<std::uint16_t>(size[1]));
}
