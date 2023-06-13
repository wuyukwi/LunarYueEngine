#include "function/ui/Widgets/Layout/Dummy.h"
#include "runtime/function/ui/Internal/Converter.h"

LunarYue::UI::Widgets::Layout::Dummy::Dummy(const Vector2& p_size) : size(p_size) {}

void LunarYue::UI::Widgets::Layout::Dummy::_Draw_Impl() { ImGui::Dummy(Internal::Converter::ToImVec2(size)); }
