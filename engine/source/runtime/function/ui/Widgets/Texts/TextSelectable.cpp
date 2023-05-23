/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "function/ui/Widgets/Texts/TextSelectable.h"

LunarYue::UI::Widgets::Texts::TextSelectable::TextSelectable(const std::string & p_content, bool p_selected, bool p_disabled) :
	Text(p_content), selected(p_selected), disabled(p_disabled)
{
}

void LunarYue::UI::Widgets::Texts::TextSelectable::_Draw_Impl()
{
	if (ImGui::Selectable((content + m_widgetID).c_str(), &selected, disabled ? ImGuiSelectableFlags_Disabled : ImGuiSelectableFlags_None))
	{
		ClickedEvent.Invoke(selected);

		if (selected)
			SelectedEvent.Invoke();
		else
			UnselectedEvent.Invoke();
	}
}
