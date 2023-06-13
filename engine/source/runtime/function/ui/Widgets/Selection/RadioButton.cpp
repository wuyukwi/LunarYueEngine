#include "function/ui/Widgets/Selection/RadioButton.h"

LunarYue::UI::Widgets::Selection::RadioButton::RadioButton(bool p_selected, const std::string& p_label) : DataWidget<bool>(m_selected), label(p_label)
{
    if (p_selected)
        Select();
}

void LunarYue::UI::Widgets::Selection::RadioButton::Select()
{
    m_selected = true;
    ClickedEvent.Invoke(m_radioID);
}

bool LunarYue::UI::Widgets::Selection::RadioButton::IsSelected() const { return m_selected; }

bool LunarYue::UI::Widgets::Selection::RadioButton::GetRadioID() const { return m_radioID; }

void LunarYue::UI::Widgets::Selection::RadioButton::_Draw_Impl()
{
    if (ImGui::RadioButton((label + m_widgetID).c_str(), m_selected))
    {
        ClickedEvent.Invoke(m_radioID);
        this->NotifyChange();
    }
}
