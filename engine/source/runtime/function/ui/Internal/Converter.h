#pragma once

#include "runtime/core/math/vector2.h"

#include "dear-imgui/imgui.h"
#include "runtime/function/ui/Types/Color.h"

namespace LunarYue::UI::Internal
{
    /**
     * Handles imgui conversion to/from overload types
     */
    class Converter
    {
    public:
        /**
         * Disabled constructor
         */
        Converter() = delete;

        /**
         * Convert the given Color to ImVec4
         * @param p_value
         */
        static ImVec4 ToImVec4(const Types::Color& p_value);

        /**
         * Convert the given ImVec4 to Color
         * @param p_value
         */
        static Types::Color ToColor(const ImVec4& p_value);

        /**
         * Convert the given FVector2 to ImVec2
         * @param p_value
         */
        static ImVec2 ToImVec2(const Vector2& p_value);

        /**
         * Convert the given ImVec2 to FVector2
         * @param p_value
         */
        static Vector2 ToFVector2(const ImVec2& p_value);
    };
} // namespace LunarYue::UI::Internal