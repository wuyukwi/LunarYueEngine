#include "function/ui/Widgets/Buttons/ButtonImage.h"
#include "runtime/function/ui/Internal/Converter.h"

LunarYue::UI::Widgets::Buttons::ButtonImage::ButtonImage(uint32_t p_textureID, const Vector2& p_size) : textureID {p_textureID}, size(p_size) {}

void LunarYue::UI::Widgets::Buttons::ButtonImage::_Draw_Impl()
{
    ImVec4 bg = Internal::Converter::ToImVec4(background);
    ImVec4 tn = Internal::Converter::ToImVec4(tint);

    if (ImGui::ImageButton(textureID.raw, Internal::Converter::ToImVec2(size), ImVec2(0.f, 1.f), ImVec2(1.f, 0.f), -1, bg, tn))
        ClickedEvent.Invoke();
}
