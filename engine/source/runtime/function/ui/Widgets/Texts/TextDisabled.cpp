#include "function/ui/Widgets/Texts/TextDisabled.h"

LunarYue::UI::Widgets::Texts::TextDisabled::TextDisabled(const std::string& p_content) : Text(p_content) {}

void LunarYue::UI::Widgets::Texts::TextDisabled::_Draw_Impl() { ImGui::TextDisabled(content.c_str()); }
