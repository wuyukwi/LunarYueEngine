/**
 * @project: Overload
 * @author: Overload Tech.
 * @licence: MIT
 */

#pragma once

#include <map>

#include "function/ui/Event/Event.h"

#include "function/ui/Widgets/DataWidget.h"

namespace LunarYue::UI::Widgets::Selection
{
    /**
     * Widget that can display a list of values that the user can select
     */
    class ComboBox : public DataWidget<int>
    {
    public:
        /**
         * Constructor
         * @param p_currentChoice
         */
        ComboBox(int p_currentChoice = 0);

    protected:
        void _Draw_Impl() override;

    public:
        std::map<int, std::string> choices;
        int                        currentChoice;

    public:
        LunarYue::Eventing::Event<int> ValueChangedEvent;
    };
} // namespace LunarYue::UI::Widgets::Selection