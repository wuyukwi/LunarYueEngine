#include "function/ui/Widgets/Drags/DragDouble.h"

LunarYue::UI::Widgets::Drags::DragDouble::DragDouble(double             p_min,
                                                     double             p_max,
                                                     double             p_value,
                                                     float              p_speed,
                                                     const std::string& p_label,
                                                     const std::string& p_format) :
    DragSingleScalar<double>(ImGuiDataType_::ImGuiDataType_Double, p_min, p_max, p_value, p_speed, p_label, p_format)
{}