/**
 * @project: Overload
 * @author: Overload Tech.
 * @licence: MIT
 */

#include "function/ui/Widgets/Buttons/Button.h"
#include "imgui_internal.h"
#include "runtime/function/ui/Internal/Converter.h"

LunarYue::UI::Widgets::Buttons::Button::Button(const std::string& p_label, const Vector2& p_size, bool p_disabled) :
    label(p_label), size(p_size), disabled(p_disabled)
{
    auto& style = ImGui::GetStyle();

    idleBackgroundColor    = Internal::Converter::ToColor(style.Colors[ImGuiCol_Button]);
    hoveredBackgroundColor = Internal::Converter::ToColor(style.Colors[ImGuiCol_ButtonHovered]);
    clickedBackgroundColor = Internal::Converter::ToColor(style.Colors[ImGuiCol_ButtonActive]);
    textColor              = Internal::Converter::ToColor(style.Colors[ImGuiCol_Text]);
}

void LunarYue::UI::Widgets::Buttons::Button::_Draw_Impl()
{
    auto& style = ImGui::GetStyle();

    auto defaultIdleColor    = style.Colors[ImGuiCol_Button];
    auto defaultHoveredColor = style.Colors[ImGuiCol_ButtonHovered];
    auto defaultClickedColor = style.Colors[ImGuiCol_ButtonActive];
    auto defaultTextColor    = style.Colors[ImGuiCol_Text];

    style.Colors[ImGuiCol_Button]        = LunarYue::UI::Internal::Converter::ToImVec4(idleBackgroundColor);
    style.Colors[ImGuiCol_ButtonHovered] = LunarYue::UI::Internal::Converter::ToImVec4(hoveredBackgroundColor);
    style.Colors[ImGuiCol_ButtonActive]  = LunarYue::UI::Internal::Converter::ToImVec4(clickedBackgroundColor);
    style.Colors[ImGuiCol_Text]          = LunarYue::UI::Internal::Converter::ToImVec4(textColor);

    if (ImGui::ButtonEx((label + m_widgetID).c_str(), Internal::Converter::ToImVec2(size), disabled ? ImGuiItemFlags_Disabled : ImGuiItemFlags_None))
        ClickedEvent.Invoke();

    style.Colors[ImGuiCol_Button]        = defaultIdleColor;
    style.Colors[ImGuiCol_ButtonHovered] = defaultHoveredColor;
    style.Colors[ImGuiCol_ButtonActive]  = defaultClickedColor;
    style.Colors[ImGuiCol_Text]          = defaultTextColor;
}
