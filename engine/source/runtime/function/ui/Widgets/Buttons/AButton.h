/**
 * @project: Overload
 * @author: Overload Tech.
 * @licence: MIT
 */

#pragma once

#include <string>

#include "function/ui/Event/Event.h"

#include "function/ui/Widgets/AWidget.h"

namespace LunarYue::UI::Widgets::Buttons
{
    /**
     * Base class for any button widget
     */
    class AButton : public AWidget
    {
    protected:
        void _Draw_Impl() override = 0;

    public:
        Eventing::Event<> ClickedEvent;
    };
} // namespace LunarYue::UI::Widgets::Buttons