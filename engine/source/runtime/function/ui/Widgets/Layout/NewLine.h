#pragma once

#include "function/ui/Widgets/AWidget.h"

namespace LunarYue::UI::Widgets::Layout
{
    /**
     * Widget that adds an empty line to the panel
     */
    class NewLine : public AWidget
    {
    protected:
        void _Draw_Impl() override;
    };
} // namespace LunarYue::UI::Widgets::Layout