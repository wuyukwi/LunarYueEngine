#pragma once

#include "function/event/event.h"

#include "function/ui/Widgets/Layout/Group.h"

namespace LunarYue::UI::Widgets::Layout
{
    /**
     * Widget that can contains other widgets and is collapsable
     */
    class GroupCollapsable : public Group
    {
    public:
        /**
         * Constructor
         * @param p_name
         */
        GroupCollapsable(const std::string& p_name = "");

    protected:
        virtual void _Draw_Impl() override;

    public:
        std::string                 name;
        bool                        closable = false;
        bool                        opened   = true;
        Event<> CloseEvent;
        Event<> OpenEvent;
    };
} // namespace LunarYue::UI::Widgets::Layout