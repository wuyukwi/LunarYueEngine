#pragma once

#include "runtime/function/ui/Panels/APanel.h"

namespace LunarYue::UI::Panels
{
    /**
     * A simple panel that will be displayed on the top side of the canvas
     */
    class PanelMenuBar : public APanel
    {
    protected:
        void _Draw_Impl() override;
    };
} // namespace LunarYue::UI::Panels