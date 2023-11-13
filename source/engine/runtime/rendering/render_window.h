#pragma once

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <core/graphics/render_pass.h>

#include <runtime/rendering/sdl_window.h>

#include <memory>

class render_window : public window_sdl
{
public:
    //-----------------------------------------------------------------------------
    //  Name : render_window ()
    /// <summary>
    /// Constructor
    /// </summary>
    //-----------------------------------------------------------------------------
    render_window();

    //-----------------------------------------------------------------------------
    //  Name : render_window ()
    /// <summary>
    /// Constructor
    /// </summary>
    //-----------------------------------------------------------------------------
    render_window(const char* title, int w, int h, std::uint32_t flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    //-----------------------------------------------------------------------------
    //  Name : ~render_window (virtual )
    /// <summary>
    /// Destructor.
    /// </summary>
    //-----------------------------------------------------------------------------
    ~render_window();

    //-----------------------------------------------------------------------------
    //  Name : get_surface ()
    /// <summary>
    /// Gets the fbo surface.
    /// </summary>
    //-----------------------------------------------------------------------------
    std::shared_ptr<gfx::frame_buffer> get_surface() const;

    //-----------------------------------------------------------------------------
    //  Name : begin_present_pass ( )
    /// <summary>
    /// Begins the present pass. Fbo -> backbufffer.
    /// </summary>
    //-----------------------------------------------------------------------------
    gfx::view_id begin_present_pass();

    //-----------------------------------------------------------------------------
    //  Name : get_id ()
    /// <summary>
    /// Gets the window id.
    /// </summary>
    //-----------------------------------------------------------------------------
    std::uint32_t get_id() const;

protected:
    //-----------------------------------------------------------------------------
    //  Name : prepare_surface (virtual )
    /// <summary>
    /// Creates the window fbo.
    /// </summary>
    //-----------------------------------------------------------------------------
    virtual void prepare_surface();

    //-----------------------------------------------------------------------------
    //  Name : destroy_surface (virtual )
    /// <summary>
    /// Destroys the window fbo.
    /// </summary>
    //-----------------------------------------------------------------------------
    virtual void destroy_surface();

    //-----------------------------------------------------------------------------
    //  Name : on_resize (virtual )
    /// <summary>
    /// This function is called so that derived classes can
    /// perform custom actions when the size of the window changes.
    /// </summary>
    //-----------------------------------------------------------------------------
    void on_resize();

    /// Window id.
    std::uint32_t id_ = 0;
    /// Render surface for this window.
    std::shared_ptr<gfx::frame_buffer> surface_;
};
