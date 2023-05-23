/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "function/ui/Widgets/Texts/Text.h"

LunarYue::UI::Widgets::Texts::Text::Text(const std::string & p_content) :
	DataWidget(content),
	content(p_content)
{
}

void LunarYue::UI::Widgets::Texts::Text::_Draw_Impl()
{
	ImGui::Text(content.c_str());
}
