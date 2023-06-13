#include "function/ui/Widgets/Texts/TextLabelled.h"

LunarYue::UI::Widgets::Texts::TextLabelled::TextLabelled(const std::string& p_content, const std::string& p_label) : Text(p_content), label(p_label)
{}

void LunarYue::UI::Widgets::Texts::TextLabelled::_Draw_Impl() { ImGui::LabelText((label + m_widgetID).c_str(), content.c_str()); }
