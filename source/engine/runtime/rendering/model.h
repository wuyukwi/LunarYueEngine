#pragma once

#include "../assets/asset_handle.h"

#include <core/common_lib/basetypes.hpp>
#include <core/graphics/graphics.h>
#include <core/math/math_includes.h>
#include <core/reflection/registration.h>
#include <core/serialization/serialization.h>

#include <vector>

class gpu_program;
class mesh;
class material;

//-----------------------------------------------------------------------------
//  Name : model (Class)
/// <summary>
/// Structure describing a LOD group(set of meshes), LOD transitions
/// and their materials.
/// </summary>
//-----------------------------------------------------------------------------
class model
{
public:
    REFLECTABLE(model)
    SERIALIZABLE(model)

    //-----------------------------------------------------------------------------
    //  Name : model ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    model();

    //-----------------------------------------------------------------------------
    //  Name : ~model (virtual )
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    virtual ~model() = default;

    //-----------------------------------------------------------------------------
    //  Name : is_valid ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    bool is_valid() const;

    //-----------------------------------------------------------------------------
    //  Name : get_lod ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    asset_handle<mesh> get_lod(std::uint32_t lod) const;

    //-----------------------------------------------------------------------------
    //  Name : set_lod ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    void set_lod(asset_handle<mesh> mesh, std::uint32_t lod);

    //-----------------------------------------------------------------------------
    //  Name : set_material ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    void set_material(asset_handle<material> material, std::uint32_t index);

    //-----------------------------------------------------------------------------
    //  Name : get_lods ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    const std::vector<asset_handle<mesh>>& get_lods() const;

    //-----------------------------------------------------------------------------
    //  Name : set_lods ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    void set_lods(const std::vector<asset_handle<mesh>>& lods);

    //-----------------------------------------------------------------------------
    //  Name : get_materials ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    const std::vector<asset_handle<material>>& get_materials() const;

    //-----------------------------------------------------------------------------
    //  Name : set_materials ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    void set_materials(const std::vector<asset_handle<material>>& materials);

    //-----------------------------------------------------------------------------
    //  Name : get_material_for_group ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    asset_handle<material> get_material_for_group(const size_t& group) const;

    //-----------------------------------------------------------------------------
    //  Name : get_lod_transition_time ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    inline float get_lod_transition_time() const { return transition_time_; }
    //-----------------------------------------------------------------------------
    //  Name : set_lod_transition_time ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    inline void set_lod_transition_time(float time) { transition_time_ = time; }
    //-----------------------------------------------------------------------------
    //  Name : get_lod_limits ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    inline const std::vector<urange32_t>& get_lod_limits() const { return lod_limits_; }
    void                                  set_lod_limits(const std::vector<urange32_t>& limits);

    //-----------------------------------------------------------------------------
    //  Name : render ()
    /// <summary>
    /// Draws a mesh with a given program. If program is nullptr then the
    /// materials are used instead. Extra states can be added to the material
    /// ones.
    /// </summary>
    //-----------------------------------------------------------------------------
    void render(gfx::view_id                        id,
                const math::transform&              world_transform,
                const std::vector<math::transform>& bone_transforms,
                bool                                apply_cull,
                bool                                depth_write,
                bool                                depth_test,
                std::uint64_t                       extra_states,
                unsigned int                        lod,
                gpu_program*                        user_program,
                std::function<void(gpu_program&)>   setup_params) const;

private:
    void recalulate_lod_limits();
    /// Collection of all materials for this model.
    std::vector<asset_handle<material>> materials_;
    /// Default material
    asset_handle<material> default_material_;
    /// Collection of all lods for this model.
    std::vector<asset_handle<mesh>> mesh_lods_;
    ///
    std::vector<urange32_t> lod_limits_;
    /// Duration for a transition between two lods.
    float transition_time_ = 0.75f;
};
