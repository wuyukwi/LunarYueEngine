#pragma once

#include "function/ui/Widgets/AWidget.h"

namespace LunarYue::UI::Widgets::Visual
{
    /**
     * Simple widget that display a bullet point
     */
    class Bullet : public AWidget
    {
    protected:
        virtual void _Draw_Impl() override;
    };
} // namespace LunarYue::UI::Widgets::Visual