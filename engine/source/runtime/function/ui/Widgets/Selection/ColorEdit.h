#pragma once

#include "function/ui/Event/Event.h"

#include "function/ui/Widgets/DataWidget.h"
#include "runtime/function/ui/Types/Color.h"

namespace LunarYue::UI::Widgets::Selection
{
    /**
     * Widget that can open a color picker on click
     */
    class ColorEdit : public DataWidget<Types::Color>
    {
    public:
        /**
         * Constructor
         * @param p_enableAlpha
         * @param p_defaultColor
         */
        ColorEdit(bool p_enableAlpha = false, const Types::Color& p_defaultColor = {});

    protected:
        void _Draw_Impl() override;

    public:
        bool                                     enableAlpha;
        Types::Color                             color;
        LunarYue::Eventing::Event<Types::Color&> ColorChangedEvent;
    };
} // namespace LunarYue::UI::Widgets::Selection