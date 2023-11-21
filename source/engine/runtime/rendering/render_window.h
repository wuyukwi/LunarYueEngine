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
    render_window(const char*   title,
                  int32_t       w,
                  int32_t       h,
                  int32_t       x     = SDL_WINDOWPOS_UNDEFINED,
                  int32_t       y     = SDL_WINDOWPOS_UNDEFINED,
                  std::uint32_t flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE,
                  uint32_t      id    = 0);

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

    /// Render surface for this window.
    std::shared_ptr<gfx::frame_buffer> surface_;
};
