#include "runtime/function/ui/Panels/APanelTransformable.h"
#include "runtime/function/ui/Internal/Converter.h"

namespace LunarYue::UI::Panels
{
    APanelTransformable::APanelTransformable(const Vector2&                 p_defaultPosition,
                                             const Vector2&                 p_defaultSize,
                                             Settings::EHorizontalAlignment p_defaultHorizontalAlignment,
                                             Settings::EVerticalAlignment   p_defaultVerticalAlignment,
                                             bool                           p_ignoreConfigFile) :
        m_defaultPosition(p_defaultPosition),
        m_defaultSize(p_defaultSize), m_defaultHorizontalAlignment(p_defaultHorizontalAlignment),
        m_defaultVerticalAlignment(p_defaultVerticalAlignment), m_ignoreConfigFile(p_ignoreConfigFile)
    {}

    void APanelTransformable::SetPosition(const Vector2& p_position)
    {
        m_position        = p_position;
        m_positionChanged = true;
    }

    void APanelTransformable::SetSize(const Vector2& p_size)
    {
        m_size        = p_size;
        m_sizeChanged = true;
    }

    void APanelTransformable::SetAlignment(Settings::EHorizontalAlignment p_horizontalAlignment, Settings::EVerticalAlignment p_verticalAligment)
    {
        m_horizontalAlignment = p_horizontalAlignment;
        m_verticalAlignment   = p_verticalAligment;
        m_alignmentChanged    = true;
    }

    const Vector2& APanelTransformable::GetPosition() const { return m_position; }

    const Vector2& APanelTransformable::GetSize() const { return m_size; }

    Settings::EHorizontalAlignment APanelTransformable::GetHorizontalAlignment() const { return m_horizontalAlignment; }

    Settings::EVerticalAlignment APanelTransformable::GetVerticalAlignment() const { return m_verticalAlignment; }

    void APanelTransformable::UpdatePosition()
    {
        if (m_defaultPosition.x != -1.f && m_defaultPosition.y != 1.f)
        {
            Vector2 offsettedDefaultPos = m_defaultPosition + CalculatePositionAlignmentOffset(true);
            ImGui::SetWindowPos(Internal::Converter::ToImVec2(offsettedDefaultPos), m_ignoreConfigFile ? ImGuiCond_Once : ImGuiCond_FirstUseEver);
        }

        if (m_positionChanged || m_alignmentChanged)
        {
            Vector2 offset = CalculatePositionAlignmentOffset(false);
            Vector2 offsettedPos(m_position.x + offset.x, m_position.y + offset.y);
            ImGui::SetWindowPos(Internal::Converter::ToImVec2(offsettedPos), ImGuiCond_Always);
            m_positionChanged  = false;
            m_alignmentChanged = false;
        }
    }

    void APanelTransformable::UpdateSize()
    {
        if (m_sizeChanged)
        {
            ImGui::SetWindowSize(Internal::Converter::ToImVec2(m_size), ImGuiCond_Always);
            m_sizeChanged = false;
        }
    }

    void APanelTransformable::CopyImGuiPosition() { m_position = Internal::Converter::ToFVector2(ImGui::GetWindowPos()); }

    void APanelTransformable::CopyImGuiSize() { m_size = Internal::Converter::ToFVector2(ImGui::GetWindowSize()); }

    void APanelTransformable::Update()
    {
        if (!m_firstFrame)
        {
            if (!autoSize)
                UpdateSize();
            CopyImGuiSize();

            UpdatePosition();
            CopyImGuiPosition();
        }

        m_firstFrame = false;
    }

    Vector2 APanelTransformable::CalculatePositionAlignmentOffset(bool p_default)
    {
        Vector2 result(0.0f, 0.0f);

        switch (p_default ? m_defaultHorizontalAlignment : m_horizontalAlignment)
        {
            case Settings::EHorizontalAlignment::CENTER:
                result.x -= m_size.x / 2.0f;
                break;
            case Settings::EHorizontalAlignment::RIGHT:
                result.x -= m_size.x;
                break;
        }

        switch (p_default ? m_defaultVerticalAlignment : m_verticalAlignment)
        {
            case Settings::EVerticalAlignment::MIDDLE:
                result.y -= m_size.y / 2.0f;
                break;
            case Settings::EVerticalAlignment::BOTTOM:
                result.y -= m_size.y;
                break;
        }

        return result;
    }
} // namespace LunarYue::UI::Panels