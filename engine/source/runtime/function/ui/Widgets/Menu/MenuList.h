#pragma once

#include "function/ui/Event/Event.h"

#include "function/ui/Widgets/Layout/Group.h"

namespace LunarYue::UI::Widgets::Menu
{
    /**
     * Widget that behave like a group with a menu display
     */
    class MenuList : public Layout::Group
    {
    public:
        /**
         * Constructor
         * @param p_name
         * @param p_locked
         */
        MenuList(const std::string& p_name, bool p_locked = false);

    protected:
        virtual void _Draw_Impl() override;

    public:
        std::string                 name;
        bool                        locked;
        LunarYue::Eventing::Event<> ClickedEvent;

    private:
        bool m_opened;
    };
} // namespace LunarYue::UI::Widgets::Menu