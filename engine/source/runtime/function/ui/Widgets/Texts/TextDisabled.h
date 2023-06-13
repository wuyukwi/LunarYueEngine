#pragma once

#include "function/ui/Widgets/Texts/Text.h"

namespace LunarYue::UI::Widgets::Texts
{
    /**
     * Widget to display a disabled text on a panel
     */
    class TextDisabled : public Text
    {
    public:
        /**
         * Constructor
         * @param p_content
         */
        TextDisabled(const std::string& p_content = "");

    protected:
        virtual void _Draw_Impl() override;
    };
} // namespace LunarYue::UI::Widgets::Texts