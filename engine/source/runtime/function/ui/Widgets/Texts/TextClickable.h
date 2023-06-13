#pragma once

#include "function/ui/Event/Event.h"

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
        LunarYue::Eventing::Event<> ClickedEvent;
        LunarYue::Eventing::Event<> DoubleClickedEvent;
    };
} // namespace LunarYue::UI::Widgets::Texts