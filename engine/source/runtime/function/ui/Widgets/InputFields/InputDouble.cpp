/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "function/ui/Widgets/InputFields/InputDouble.h"

LunarYue::UI::Widgets::InputFields::InputDouble::InputDouble(double p_defaultValue, double p_step, double p_fastStep, const std::string & p_label, const std::string & p_format, bool p_selectAllOnClick)
	: InputSingleScalar<double>(ImGuiDataType_::ImGuiDataType_Double, p_defaultValue, p_step, p_fastStep, p_label, p_format, p_selectAllOnClick)
{
}
