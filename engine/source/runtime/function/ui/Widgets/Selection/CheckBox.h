#pragma once

#include "function/event/event.h"

#include "function/ui/Widgets/DataWidget.h"

namespace LunarYue::UI::Widgets::Selection
{
    /**
     * Checkbox widget that can be checked or not
     */
    class CheckBox : public DataWidget<bool>
    {
    public:
        /**
         * Constructor
         * @param p_value
         * @param p_label
         */
        CheckBox(bool p_value = false, const std::string& p_label = "");

    protected:
        void _Draw_Impl() override;

    public:
        bool                            value;
        std::string                     label;
        Event<bool> ValueChangedEvent;
    };
} // namespace LunarYue::UI::Widgets::Selection