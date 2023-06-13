#pragma once

#include <stdint.h>

namespace LunarYue::UI::Internal
{
    /**
     * Simple union necessary for imgui textureID
     */
    union TextureID
    {
        uint32_t id;
        void*    raw;
    };
} // namespace LunarYue::UI::Internal