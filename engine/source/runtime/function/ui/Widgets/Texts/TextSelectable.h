/**
 * @project: Overload
 * @author: Overload Tech.
 * @licence: MIT
 */

#pragma once

#include "function/ui/Event/Event.h"

#include "function/ui/Widgets/Texts/Text.h"

namespace LunarYue::UI::Widgets::Texts
{
    /**
     * Simple widget to display a selectable text on a panel
     */
    class TextSelectable : public Text
    {
    public:
        /**
         * Constructor
         * @param p_content
         * @param p_selected
         * @param p_disabled
         */
        TextSelectable(const std::string& p_content = "", bool p_selected = false, bool p_disabled = false);

    protected:
        virtual void _Draw_Impl() override;

    public:
        bool selected;
        bool disabled;

        LunarYue::Eventing::Event<bool> ClickedEvent;
        LunarYue::Eventing::Event<>     SelectedEvent;
        LunarYue::Eventing::Event<>     UnselectedEvent;
    };
} // namespace LunarYue::UI::Widgets::Texts