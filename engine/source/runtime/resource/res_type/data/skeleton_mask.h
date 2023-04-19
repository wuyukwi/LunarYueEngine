#pragma once
#include "runtime/core/meta/reflection/reflection.h"
#include <string>
#include <vector>
namespace LunarYue
{

    REFLECTION_TYPE(BoneBlendMask)
    CLASS(BoneBlendMask, Fields)
    {
        REFLECTION_BODY(BoneBlendMask);

    public:
        std::string      skeleton_file_path;
        std::vector<int> enabled;
    };

} // namespace LunarYue