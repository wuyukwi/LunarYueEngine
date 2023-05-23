/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "function/ui/Widgets/Texts/TextColored.h"
#include "runtime/function/ui/Internal/Converter.h"

LunarYue::UI::Widgets::Texts::TextColored::TextColored(const std::string& p_content, const Types::Color& p_color) :
	Text(p_content), color(p_color)
{
}

void LunarYue::UI::Widgets::Texts::TextColored::_Draw_Impl()
{
	ImGui::TextColored(Internal::Converter::ToImVec4(color), content.c_str());
}
