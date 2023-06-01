#pragma once

#include "function/ui/Widgets/AWidget.h"

namespace LunarYue::UI::Widgets::Visual
{
    /**
     * Simple widget that display a separator
     */
    class Separator : public AWidget
    {
    protected:
        void _Draw_Impl() override;
    };
} // namespace LunarYue::UI::Widgets::Visual