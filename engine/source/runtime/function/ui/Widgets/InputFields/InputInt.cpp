#include "function/ui/Widgets/InputFields/InputInt.h"

LunarYue::UI::Widgets::InputFields::InputInt::InputInt(int                p_defaultValue,
                                                       int                p_step,
                                                       int                p_fastStep,
                                                       const std::string& p_label,
                                                       const std::string& p_format,
                                                       bool               p_selectAllOnClick) :
    InputSingleScalar<int>(ImGuiDataType_::ImGuiDataType_S32, p_defaultValue, p_step, p_fastStep, p_label, p_format, p_selectAllOnClick)
{}
