#pragma once
//-----------------------------------------------------------------------------
// camera_component Header Includes
//-----------------------------------------------------------------------------
#include "../../rendering/camera.h"
#include "../ecs.h"

#include <core/common_lib/basetypes.hpp>
#include <core/graphics/render_pass.h>
#include <core/graphics/render_view.h>
#include <core/math/math_includes.h>
//-----------------------------------------------------------------------------
// Forward Declarations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Main Class Declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Name : camera_component (Class)
/// <summary>
/// Class that contains our core camera data, used for rendering and other
/// things.
/// </summary>
//-----------------------------------------------------------------------------
class camera_component : public runtime::component_impl<camera_component>
{
    SERIALIZABLE(camera_component)
    REFLECTABLEV(camera_component, runtime::component)
public:
    //-------------------------------------------------------------------------
    // Constructors & Destructors
    //-------------------------------------------------------------------------
    camera_component();
    virtual ~camera_component();

    //-------------------------------------------------------------------------
    // Public Methods
    //-------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    //  Name : set_fov ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    void set_fov(float fovDegrees);

    //-----------------------------------------------------------------------------
    //  Name : set_near_clip ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    void set_near_clip(float distance);

    //-----------------------------------------------------------------------------
    //  Name : set_far_clip ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    void set_far_clip(float distance);

    //-----------------------------------------------------------------------------
    //  Name : set_projection_mode ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    void set_projection_mode(projection_mode mode);

    //-----------------------------------------------------------------------------
    //  Name : get_fov ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    float get_fov() const;

    //-----------------------------------------------------------------------------
    //  Name : get_near_clip ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    float get_near_clip() const;

    //-----------------------------------------------------------------------------
    //  Name : get_far_clip ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    float get_far_clip() const;

    //-----------------------------------------------------------------------------
    //  Name : get_projection_mode ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    projection_mode get_projection_mode() const;

    //-----------------------------------------------------------------------------
    //  Name : get_camera ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    inline camera& get_camera() { return camera_; }

    //-----------------------------------------------------------------------------
    //  Name : get_camera ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    inline const camera& get_camera() const { return camera_; }

    //-----------------------------------------------------------------------------
    //  Name : update ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    void update(const math::transform& t);

    //-----------------------------------------------------------------------------
    //  Name : get_hdr ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    bool get_hdr() const;

    //-----------------------------------------------------------------------------
    //  Name : set_hdr ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    void set_hdr(bool hdr);

    //-----------------------------------------------------------------------------
    //  Name : set_viewport_size ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    void set_viewport_size(const usize32_t& size);

    //-----------------------------------------------------------------------------
    //  Name : get_viewport_size ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    const usize32_t& get_viewport_size() const;

    //-----------------------------------------------------------------------------
    //  Name : get_ortho_size ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    float get_ortho_size() const;

    //-----------------------------------------------------------------------------
    //  Name : set_ortho_size ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    void set_ortho_size(float size);

    //-----------------------------------------------------------------------------
    //  Name : get_ppu ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    float get_ppu() const;

    //-----------------------------------------------------------------------------
    //  Name : get_render_view ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    inline gfx::render_view& get_render_view() { return render_view_; }

private:
    //-------------------------------------------------------------------------
    // Private Member Variables.
    //-------------------------------------------------------------------------
    /// The camera object this component represents
    camera camera_;
    /// The render view for this component
    gfx::render_view render_view_;
    /// Is the camera HDR?
    bool hdr_ = true;
};