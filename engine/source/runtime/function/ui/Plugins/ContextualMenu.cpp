#include "runtime/function/ui/Plugins/ContextualMenu.h"

void LunarYue::UI::Plugins::ContextualMenu::Execute()
{
    if (ImGui::BeginPopupContextItem())
    {
        DrawWidgets();
        ImGui::EndPopup();
    }
}

void LunarYue::UI::Plugins::ContextualMenu::Close() { ImGui::CloseCurrentPopup(); }
