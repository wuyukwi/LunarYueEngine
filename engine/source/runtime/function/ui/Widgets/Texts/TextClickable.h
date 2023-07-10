#pragma once

#include "function/event/event.h" #include "function/event/event.h"

#include "function/ui/Widgets/Texts/Text.h"

namespace LunarYue::UI::Widgets::Texts
{
    /**
     * Widget to display text on a panel that is also clickable
     */
    class TextClickable : public Text
    {
    public:
        /**
         * Constructor
         * @param p_content
         */
        TextClickable(const std::string& p_content = "");

    protected:
        virtual void _Draw_Impl() override;

    public:
        Event<> ClickedEvent;
        Event<> DoubleClickedEvent;
    };
} // namespace LunarYue::UI::Widgets::Texts