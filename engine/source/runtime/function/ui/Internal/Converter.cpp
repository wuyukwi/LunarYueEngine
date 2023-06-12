#include "runtime/function/ui/Internal/Converter.h"

ImVec4 LunarYue::UI::Internal::Converter::ToImVec4(const Types::Color& p_value) { return {p_value.r, p_value.g, p_value.b, p_value.a}; }

LunarYue::UI::Types::Color LunarYue::UI::Internal::Converter::ToColor(const ImVec4& p_value) { return {p_value.x, p_value.y, p_value.z, p_value.w}; }

ImVec2 LunarYue::UI::Internal::Converter::ToImVec2(const Vector2& p_value) { return {p_value.x, p_value.y}; }

LunarYue::Vector2 LunarYue::UI::Internal::Converter::ToFVector2(const ImVec2& p_value) { return {p_value.x, p_value.y}; }
