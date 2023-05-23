/**
 * @project: Overload
 * @author: Overload Tech.
 * @licence: MIT
 */

#pragma once

#include "function/ui/Event/Event.h"

#include "function/ui/Widgets/DataWidget.h"

namespace LunarYue::UI::Widgets::Menu
{
    /**
     * Widget that can be added to a menu list. It is clickable and can be checkable
     */
    class MenuItem : public DataWidget<bool>
    {
    public:
        /**
         * Constructor
         * @param p_name
         * @param p_shortcut
         * @param p_checkable
         * @param p_checked
         */
        MenuItem(const std::string& p_name, const std::string& p_shortcut = "", bool p_checkable = false, bool p_checked = false);

    protected:
        void _Draw_Impl() override;

    public:
        std::string           name;
        std::string           shortcut;
        bool                  checkable;
        bool                  checked;
        Eventing::Event<>     ClickedEvent;
        Eventing::Event<bool> ValueChangedEvent;

    private:
        bool m_selected;
    };
} // namespace LunarYue::UI::Widgets::Menu