#include "function/ui/Widgets/Texts/TextClickable.h"

LunarYue::UI::Widgets::Texts::TextClickable::TextClickable(const std::string& p_content) : Text(p_content) {}

void LunarYue::UI::Widgets::Texts::TextClickable::_Draw_Impl()
{
    bool useless = false;

    if (ImGui::Selectable((content + m_widgetID).c_str(), &useless, ImGuiSelectableFlags_AllowDoubleClick))
    {
        if (ImGui::IsMouseDoubleClicked(0))
        {
            DoubleClickedEvent.Invoke();
        }
        else
        {
            ClickedEvent.Invoke();
        }
    }
}
