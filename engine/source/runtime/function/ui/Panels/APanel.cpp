#include "runtime/function/ui/Panels/APanel.h"

uint64_t LunarYue::UI::Panels::APanel::__PANEL_ID_INCREMENT = 0;

LunarYue::UI::Panels::APanel::APanel() { m_panelID = "##" + std::to_string(__PANEL_ID_INCREMENT++); }

void LunarYue::UI::Panels::APanel::Draw()
{
    if (enabled)
        _Draw_Impl();
}

const std::string& LunarYue::UI::Panels::APanel::GetPanelID() const { return m_panelID; }
