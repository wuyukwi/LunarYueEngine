#pragma once

#include "function/ui/Widgets/Drags/DragMultipleScalars.h"

namespace LunarYue::UI::Widgets::Drags
{
    /**
     * Drag widget of multiple type double
     */
    template<size_t _Size>
    class DragMultipleDoubles : public DragMultipleScalars<double, _Size>
    {
    public:
        /**
         * Constructor
         * @param p_min
         * @param p_max
         * @param p_value
         * @param p_speed
         * @param p_label
         * @param p_format
         */
        DragMultipleDoubles(double             p_min    = 0.0,
                            double             p_max    = 1.0,
                            double             p_value  = 0.5,
                            float              p_speed  = 0.1f,
                            const std::string& p_label  = "",
                            const std::string& p_format = "%.6f") :
            DragMultipleScalars<double, _Size>(ImGuiDataType_::ImGuiDataType_Double, p_min, p_max, p_value, p_speed, p_label, p_format)
        {}
    };
} // namespace LunarYue::UI::Widgets::Drags