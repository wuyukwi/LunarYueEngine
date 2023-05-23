/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "function/ui/Widgets/Visual/ProgressBar.h"
#include "runtime/function/ui/Internal/Converter.h"

LunarYue::UI::Widgets::Visual::ProgressBar::ProgressBar(float p_fraction, const Vector2 & p_size, const std::string & p_overlay) :
	fraction(p_fraction), size(p_size), overlay(p_overlay)
{
}

void LunarYue::UI::Widgets::Visual::ProgressBar::_Draw_Impl()
{
	ImGui::ProgressBar(fraction, Internal::Converter::ToImVec2(size), !overlay.empty() ? overlay.c_str() : nullptr);
}
