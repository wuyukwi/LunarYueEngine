#pragma once

#include "../ecs.h"

#include <core/common_lib/basetypes.hpp>

#include <vector>

namespace runtime
{
    class scene_graph
    {
    public:
        scene_graph();
        ~scene_graph();
        //-----------------------------------------------------------------------------
        //  Name : frame_update (virtual )
        /// <summary>
        ///
        ///
        ///
        /// </summary>
        //-----------------------------------------------------------------------------
        void frame_update(float dt);

        //-----------------------------------------------------------------------------
        //  Name : getRoots ()
        /// <summary>
        ///
        ///
        ///
        /// </summary>
        //-----------------------------------------------------------------------------
        const std::vector<entity>& get_roots() const { return roots_; }

    private:
        /// scene roots
        std::vector<entity> roots_;
    };
} // namespace runtime
