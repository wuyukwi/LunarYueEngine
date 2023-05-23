/**
 * @project: Overload
 * @author: Overload Tech.
 * @licence: MIT
 */

#include "function/ui/Widgets/Layout/GroupCollapsable.h"
#include "imgui_internal.h"

LunarYue::UI::Widgets::Layout::GroupCollapsable::GroupCollapsable(const std::string& p_name) : name(p_name) {}

void LunarYue::UI::Widgets::Layout::GroupCollapsable::_Draw_Impl()
{
    bool previouslyOpened = opened;

    if (ImGui::CollapsingHeader(name.c_str(), closable ? &opened : nullptr))
        Group::_Draw_Impl();

    if (opened != previouslyOpened)
    {
        if (opened)
            OpenEvent.Invoke();
        else
            CloseEvent.Invoke();
    }
}
