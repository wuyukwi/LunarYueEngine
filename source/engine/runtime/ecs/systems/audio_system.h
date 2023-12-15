#pragma once

#include <core/common_lib/basetypes.hpp>

namespace runtime
{
    class audio_system
    {
    public:
        audio_system();
        ~audio_system();
        //-----------------------------------------------------------------------------
        //  Name : frame_update (virtual )
        /// <summary>
        ///
        ///
        ///
        /// </summary>
        //-----------------------------------------------------------------------------
        void frame_update(float dt);
    };
} // namespace runtime
