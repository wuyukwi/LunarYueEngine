#include "function/ui/Widgets/Layout/Spacing.h"

LunarYue::UI::Widgets::Layout::Spacing::Spacing(uint16_t p_spaces) : spaces(p_spaces) {}

void LunarYue::UI::Widgets::Layout::Spacing::_Draw_Impl()
{
    for (uint16_t i = 0; i < spaces; ++i)
    {
        ImGui::Spacing();

        if (i + 1 < spaces)
            ImGui::SameLine();
    }
}
