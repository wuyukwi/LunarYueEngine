#pragma once

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <core/graphics/render_pass.h>

#include <runtime/rendering/sdl_window.h>

#include <memory>

class render_window : public sdl_window
{
public:
    //-----------------------------------------------------------------------------
    //  Name : render_window ()
    /// <summary>
    /// Constructor
    /// </summary>
    //-----------------------------------------------------------------------------
    render_window(const char* title, int32_t w, int32_t h, int32_t x = -1, int32_t y = -1, std::uint32_t flags = 0);

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

    gfx::view_id get_view_id();

    void set_size(const std::array<std::uint32_t, 2>& size) override;

    //-----------------------------------------------------------------------------
    //  Name : on_resize (virtual )
    /// <summary>
    /// This function is called so that derived classes can
    /// perform custom actions when the size of the window changes.
    /// </summary>
    //-----------------------------------------------------------------------------
    void on_resize();

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

    /// Render surface for this window.
    std::shared_ptr<gfx::frame_buffer> surface_;
    gfx::view_id                       view_id_;
};
