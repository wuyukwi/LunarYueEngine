#pragma once

#include <limits>
#include <vector>

#include "runtime/core/math/vector2.h"

#include "function/ui/Widgets/DataWidget.h"

namespace LunarYue::UI::Widgets::Plots
{
    /**
     * Base class for any plot widget
     */
    class APlot : public DataWidget<std::vector<float>>
    {
    public:
        /**
         * Constructor
         * @param p_data
         * @param p_minScale
         * @param p_maxScale
         * @param p_size
         * @param p_overlay
         * @param p_label
         * @param p_forceHover
         */
        APlot(const std::vector<float>& p_data       = std::vector<float>(),
              float                     p_minScale   = std::numeric_limits<float>::min(),
              float                     p_maxScale   = std::numeric_limits<float>::max(),
              const Vector2&            p_size       = {0.0f, 0.0f},
              const std::string&        p_overlay    = "",
              const std::string&        p_label      = "",
              int                       p_forceHover = -1);

    protected:
        virtual void _Draw_Impl() override = 0;

    public:
        std::vector<float> data;
        float              minScale;
        float              maxScale;
        Vector2            size;
        std::string        overlay;
        std::string        label;
        int                forceHover;
    };
} // namespace LunarYue::UI::Widgets::Plots