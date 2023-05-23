/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "function/ui/Widgets/Buttons/ButtonArrow.h"

LunarYue::UI::Widgets::Buttons::ButtonArrow::ButtonArrow(ImGuiDir p_direction) :
	direction(p_direction)
{
}

void LunarYue::UI::Widgets::Buttons::ButtonArrow::_Draw_Impl()
{
	if (ImGui::ArrowButton(m_widgetID.c_str(), direction))
		ClickedEvent.Invoke();
}
