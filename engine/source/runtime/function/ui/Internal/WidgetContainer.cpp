/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <algorithm>

#include "runtime/function/ui/Internal/WidgetContainer.h"

void LunarYue::UI::Internal::WidgetContainer::RemoveWidget(Widgets::AWidget& p_widget)
{
	auto found = std::find_if(m_widgets.begin(), m_widgets.end(), [&p_widget](std::pair<LunarYue::UI::Widgets::AWidget*, Internal::EMemoryMode>& p_pair)
	{ 
		return p_pair.first == &p_widget;
	});

	if (found != m_widgets.end())
	{
		if (found->second == Internal::EMemoryMode::INTERNAL_MANAGMENT)
			delete found->first;

		m_widgets.erase(found);
	}
}

void LunarYue::UI::Internal::WidgetContainer::RemoveAllWidgets()
{
	std::for_each(m_widgets.begin(), m_widgets.end(), [](auto& pair)
	{
		if (pair.second == Internal::EMemoryMode::INTERNAL_MANAGMENT)
			delete pair.first;
	});

	m_widgets.clear();
}

void LunarYue::UI::Internal::WidgetContainer::ConsiderWidget(Widgets::AWidget & p_widget, bool p_manageMemory)
{
	m_widgets.emplace_back(std::make_pair(&p_widget, p_manageMemory ? EMemoryMode::INTERNAL_MANAGMENT : EMemoryMode::EXTERNAL_MANAGMENT));
	p_widget.SetParent(this);
}

void LunarYue::UI::Internal::WidgetContainer::UnconsiderWidget(Widgets::AWidget & p_widget)
{
	auto found = std::find_if(m_widgets.begin(), m_widgets.end(), [&p_widget](std::pair<LunarYue::UI::Widgets::AWidget*, Internal::EMemoryMode>& p_pair)
	{
		return p_pair.first == &p_widget;
	});

	if (found != m_widgets.end())
	{
		p_widget.SetParent(nullptr);
		m_widgets.erase(found);
	}
}

void LunarYue::UI::Internal::WidgetContainer::CollectGarbages()
{
	m_widgets.erase(std::remove_if(m_widgets.begin(), m_widgets.end(), [](std::pair<LunarYue::UI::Widgets::AWidget*, Internal::EMemoryMode>& p_item)
	{
		bool toDestroy = p_item.first && p_item.first->IsDestroyed();

		if (toDestroy && p_item.second == Internal::EMemoryMode::INTERNAL_MANAGMENT)
			delete p_item.first;

		return toDestroy;
	}), m_widgets.end());
}

void LunarYue::UI::Internal::WidgetContainer::DrawWidgets()
{
	CollectGarbages();

    if (m_reversedDrawOrder)
    {
        for (auto it = m_widgets.crbegin(); it != m_widgets.crend(); ++it)
            it->first->Draw();
    }
    else
    {
        for (const auto& widget : m_widgets)
            widget.first->Draw();
    }
}

void LunarYue::UI::Internal::WidgetContainer::ReverseDrawOrder(const bool reversed)
{
    m_reversedDrawOrder = reversed;
}

std::vector<std::pair<LunarYue::UI::Widgets::AWidget*, LunarYue::UI::Internal::EMemoryMode>>& LunarYue::UI::Internal::WidgetContainer::GetWidgets()
{
	return m_widgets;
}
