#pragma once

#include "function/ui/Widgets/AWidget.h"
#include "function/ui/Widgets/DataWidget.h"

namespace LunarYue::UI::Widgets::Texts
{
    /**
     * Simple widget to display text on a panel
     */
    class Text : public DataWidget<std::string>
    {
    public:
        /**
         * Constructor
         * @param p_content
         */
        Text(const std::string& p_content = "");

    protected:
        virtual void _Draw_Impl() override;

    public:
        std::string content;
    };
} // namespace LunarYue::UI::Widgets::Texts