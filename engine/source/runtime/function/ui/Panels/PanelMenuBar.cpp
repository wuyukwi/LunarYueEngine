/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "runtime/function/ui/Panels/PanelMenuBar.h"

void LunarYue::UI::Panels::PanelMenuBar::_Draw_Impl()
{
	if (!m_widgets.empty() && ImGui::BeginMainMenuBar())
	{
		DrawWidgets();
		ImGui::EndMainMenuBar();
	}
}