#include "function/ui/Widgets/AWidget.h"

namespace LunarYue
{
    uint64_t UI::Widgets::AWidget::__WIDGET_ID_INCREMENT = 0;

    UI::Widgets::AWidget::AWidget() { m_widgetID = "##" + std::to_string(__WIDGET_ID_INCREMENT++); }

    void UI::Widgets::AWidget::LinkTo(const AWidget& p_widget) { m_widgetID = p_widget.m_widgetID; }

    void UI::Widgets::AWidget::Destroy() { m_destroyed = true; }

    bool UI::Widgets::AWidget::IsDestroyed() const { return m_destroyed; }

    void UI::Widgets::AWidget::SetParent(Internal::WidgetContainer* p_parent) { m_parent = p_parent; }

    bool UI::Widgets::AWidget::HasParent() const { return m_parent; }

    UI::Internal::WidgetContainer* UI::Widgets::AWidget::GetParent() { return m_parent; }

    void UI::Widgets::AWidget::Draw()
    {
        if (enabled)
        {
            _Draw_Impl();

            if (m_autoExecutePlugins)
                ExecutePlugins();

            if (!lineBreak)
                ImGui::SameLine();
        }
    }
} // namespace LunarYue
