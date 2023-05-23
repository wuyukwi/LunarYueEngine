/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include "function/ui/Widgets/AWidget.h"

uint64_t LunarYue::UI::Widgets::AWidget::__WIDGET_ID_INCREMENT = 0;

LunarYue::UI::Widgets::AWidget::AWidget()
{
	m_widgetID = "##" + std::to_string(__WIDGET_ID_INCREMENT++);
}

void LunarYue::UI::Widgets::AWidget::LinkTo(const AWidget& p_widget)
{
	m_widgetID = p_widget.m_widgetID;
}

void LunarYue::UI::Widgets::AWidget::Destroy()
{
	m_destroyed = true;
}

bool LunarYue::UI::Widgets::AWidget::IsDestroyed() const
{
	return m_destroyed;
}

void LunarYue::UI::Widgets::AWidget::SetParent(Internal::WidgetContainer * p_parent)
{
	m_parent = p_parent;
}

bool LunarYue::UI::Widgets::AWidget::HasParent() const
{
	return m_parent;
}

LunarYue::UI::Internal::WidgetContainer * LunarYue::UI::Widgets::AWidget::GetParent()
{
	return m_parent;
}

void LunarYue::UI::Widgets::AWidget::Draw()
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
