#pragma once

#include "function/ui/Widgets/Selection/RadioButton.h"

namespace LunarYue::UI::Widgets::Selection
{
    /**
     * Handle the link of radio buttons. Necessary to enable the correct behaviour
     * of radio buttons
     */
    class RadioButtonLinker : public DataWidget<int>
    {
    public:
        /**
         * Constructor
         */
        RadioButtonLinker();

        /**
         * Link the given radio button
         */
        void Link(RadioButton& p_radioButton);

        /**
         * Unlink the given radio button
         */
        void Unlink(RadioButton& p_radioButton);

        /**
         * Returns the id of the selected radio button
         */
        int GetSelected() const;

    protected:
        void _Draw_Impl() override;

    private:
        void OnRadioButtonClicked(int p_radioID);

    public:
        Event<int> ValueChangedEvent;

    private:
        int                                                                                         m_availableRadioID = 0;
        int                                                                                         m_selected         = -1;
        std::vector<std::pair<LunarYue::Eventing::ListenerID, std::reference_wrapper<RadioButton>>> m_radioButtons;
    };
} // namespace LunarYue::UI::Widgets::Selection