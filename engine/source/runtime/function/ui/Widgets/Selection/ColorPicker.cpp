#include "function/ui/Widgets/Selection/ColorPicker.h"

LunarYue::UI::Widgets::Selection::ColorPicker::ColorPicker(bool p_enableAlpha, const Types::Color& p_defaultColor) :
    DataWidget<Types::Color>(color), enableAlpha(p_enableAlpha), color(p_defaultColor)
{}

void LunarYue::UI::Widgets::Selection::ColorPicker::_Draw_Impl()
{
    int  flags        = !enableAlpha ? ImGuiColorEditFlags_NoAlpha : 0;
    bool valueChanged = false;

    if (enableAlpha)
        valueChanged = ImGui::ColorPicker4(m_widgetID.c_str(), &color.r, flags);
    else
        valueChanged = ImGui::ColorPicker3(m_widgetID.c_str(), &color.r, flags);

    if (valueChanged)
    {
        ColorChangedEvent.Invoke(color);
        this->NotifyChange();
    }
}
