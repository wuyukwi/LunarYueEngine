#include "renderer.h"

#include "../system/events.h"

#include <core/common_lib/assert.hpp>
#include <core/graphics/graphics.h>
#include <core/graphics/render_pass.h>
#include <core/logging/logging.h>

#include <SDL.h>

#include <algorithm>
#include <cstdarg>

namespace runtime
{
    renderer::renderer(cmd_line::parser& parser)
    {
        on_platform_events.connect(this, &renderer::platform_events);
        on_frame_end.connect(this, &renderer::frame_end);

        if (!init_backend(parser))
        {
            return;
        }

        auto window = std::make_unique<render_window>("LunarYue", 1280, 720);
        window->raise_window();
        register_window(std::move(window));
    }

    renderer::~renderer()
    {
        on_platform_events.disconnect(this, &renderer::platform_events);
        on_frame_end.disconnect(this, &renderer::frame_end);
        windows_.clear();
        gfx::shutdown();
        SDL_Quit();
    }

    const std::unique_ptr<render_window>& renderer::get_focused_window() const
    {
        static const std::unique_ptr<render_window> empty;

        const auto& windows = get_windows();
        auto        it      = std::find_if(std::begin(windows), std::end(windows), [](const auto& window) { return window->has_focus(); });

        if (it != std::end(windows))
        {
            return *it;
        }

        return empty;
    }

    void renderer::register_window(std::unique_ptr<render_window> window) { windows_.emplace_back(std::move(window)); }

    void renderer::remove_window_by_id(uint32_t id)
    {
        auto it = std::remove_if(std::begin(windows_), std::end(windows_), [id](const auto& window) { return window->get_sdl_window_id() == id; });

        windows_.erase(it, std::end(windows_));
    }

    const std::vector<std::unique_ptr<render_window>>& renderer::get_windows() const { return windows_; }

    const std::unique_ptr<render_window>& renderer::get_window_for_id(std::uint32_t id) const
    {
        auto it = std::find_if(std::begin(windows_), std::end(windows_), [id](const auto& window) { return window->get_sdl_window_id() == id; });

        ensures(it != std::end(windows_));

        return *it;
    }

    const std::unique_ptr<render_window>& renderer::get_main_window() const
    {
        expects(!windows_.empty());

        return windows_.front();
    }

    void renderer::hide_all_secondary_windows()
    {
        std::size_t i = 0;
        for (auto& window : windows_)
        {
            if (i++ != 0)
            {
                window->set_visible(false);
            }
        }
    }

    void renderer::show_all_secondary_windows()
    {
        std::size_t i = 0;
        for (auto& window : windows_)
        {
            if (i++ != 0)
            {
                window->set_visible(true);
            }
        }
    }

    void renderer::platform_events(const std::vector<SDL_Event>& events)
    {
        for (const auto& e : events)
        {
            if (e.type == SDL_QUIT)
                should_quit_ = true;

            if (e.type == SDL_WINDOWEVENT)
            {
                if (e.window.event == SDL_WINDOWEVENT_CLOSE)
                {
                    if (e.window.windowID == get_main_window()->get_sdl_window_id())
                    {
                        should_quit_ = true;
                    }
                    else
                    {
                        windows_.erase(
                            std::remove_if(std::begin(windows_),
                                           std::end(windows_),
                                           [window_id = e.window.windowID](const auto& window) { return window->get_sdl_window_id() == window_id; }),
                            std::end(windows_));
                    }
                    return;
                }
            }
            if (e.window.event == SDL_WINDOWEVENT_RESIZED)
            {
                auto it = std::find_if(std::begin(windows_), std::end(windows_), [window_id = e.window.windowID](const auto& window) {
                    return window->get_sdl_window_id() == window_id;
                });

                if (it != std::end(windows_))
                {
                    it->get()->on_resize();
                }
            }
        }
    }

    bool renderer::init_backend(cmd_line::parser& parser)
    {
        if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) > 0)
        {
            APPLOG_ERROR("Couldn't initialize SDL: {}", SDL_GetError());
            return false;
        }
        SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
        SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
        SDL_SetHint(SDL_HINT_MOUSE_AUTO_CAPTURE, "0");

        std::unique_ptr<sdl_window> init_window = std::make_unique<sdl_window>("init window", 100, 100, 100, 0, 0);

        auto size = init_window->get_size();
        init_window->set_visible(false);
        //	gfx::platform_data pd;
        //	pd.ndt = init_window_->native_display_handle();
        //	pd.nwh = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(init_window_->native_handle()));

        ////	gfx::set_platform_data(pd);

        // auto detect
        auto preferred_renderer_type = gfx::renderer_type::Count;

        std::string preferred_renderer;
        if (parser.try_get("renderer", preferred_renderer))
        {
            if (preferred_renderer == "opengl")
            {
                preferred_renderer_type = gfx::renderer_type::OpenGL;
            }
            else if (preferred_renderer == "vulkan")
            {
                preferred_renderer_type = gfx::renderer_type::Vulkan;
            }
            else if (preferred_renderer == "directx11")
            {
                preferred_renderer_type = gfx::renderer_type::Direct3D11;
            }
            else if (preferred_renderer == "directx12")
            {
                preferred_renderer_type = gfx::renderer_type::Direct3D12;
            }
        }

        gfx::init_type init_data;
        init_data.type              = preferred_renderer_type;
        init_data.resolution.width  = size[0];
        init_data.resolution.height = size[1];
        init_data.resolution.reset  = BGFX_RESET_VSYNC;
        // init_data.platformData.ndt = init_data.platformData.ndt = init_window_->native_display_handle();
        init_data.platformData.nwh = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(init_window->get_native_window_handle()));

        bool novsync = false;
        parser.try_get("novsync", novsync);
        if (novsync)
        {
            init_data.resolution.reset = 0;
        }
        if (!gfx::init(init_data))
        {
            APPLOG_ERROR("Could not initialize rendering backend!");
            return false;
        }
        if (gfx::get_renderer_type() == gfx::renderer_type::Direct3D9)
        {
            APPLOG_ERROR("Does not support dx9. Minimum supported is dx11.");
            return false;
        }

        APPLOG_INFO("Using {0} rendering backend.", gfx::get_renderer_name(gfx::get_renderer_type()));

        if (gfx::get_renderer_type() == gfx::renderer_type::Direct3D12)
        {
            APPLOG_WARNING("Directx 12 support is experimental and unstable.");
        }

        init_window.reset();

        return true;
    }

    void renderer::frame_end(delta_t /*unused*/)
    {
        gfx::render_pass pass("init_bb_update");
        pass.bind();
        pass.clear();

        render_frame_ = gfx::frame();

        gfx::render_pass::reset();
    }
} // namespace runtime
