#include "function/ui/Widgets/Texts/TextWrapped.h"

LunarYue::UI::Widgets::Texts::TextWrapped::TextWrapped(const std::string& p_content) : Text(p_content) {}

void LunarYue::UI::Widgets::Texts::TextWrapped::_Draw_Impl() { ImGui::TextWrapped(content.c_str()); }
