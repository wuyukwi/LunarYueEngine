////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "render_window.h"

void render_window::on_resize() { render_window::prepare_surface(); }

void render_window::set_size(const std::array<std::uint32_t, 2>& size)
{
    sdl_window::set_size(size);
    on_resize();
}

render_window::render_window(const char* title, int32_t w, int32_t h, int32_t x, int32_t y, std::uint32_t flags) :
    sdl_window(title, w, h, x, y, flags)
{
    render_window::prepare_surface();
}

render_window::~render_window()
{
    sdl_window::~sdl_window();

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
    const gfx::render_pass pass(get_title());
    pass.bind(surface_.get());
    pass.clear();

    view_id_ = pass.id;

    return view_id_;
}

gfx::view_id render_window::get_view_id() { return view_id_; }

void render_window::prepare_surface()
{
    const auto size = get_drawable_size();

    surface_ = std::make_shared<gfx::frame_buffer>(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(get_native_window_handle())),
                                                   static_cast<std::uint16_t>(size[0]),
                                                   static_cast<std::uint16_t>(size[1]));
}
