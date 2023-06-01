#pragma once

#include <vector>

#include "function/ui/Widgets/AWidget.h"
#include "runtime/function/ui/Internal/EMemoryMode.h"

namespace LunarYue::UI::Internal
{
    /**
     * The base class for any widget container
     */
    class WidgetContainer
    {
    public:
        /**
         * Remove a widget from the container
         */
        void RemoveWidget(Widgets::AWidget& p_widget);

        /**
         * Remove all widgets from the container
         */
        void RemoveAllWidgets();

        /**
         * Consider a widget
         */
        void ConsiderWidget(Widgets::AWidget& p_widget, bool p_manageMemory = true);

        /**
         * Unconsider a widget
         */
        void UnconsiderWidget(Widgets::AWidget& p_widget);

        /**
         * Collect garbages by removing widgets marked as "Destroyed"
         */
        void CollectGarbages();

        /**
         * Draw every widgets
         */
        void DrawWidgets();

        /**
         * Allow the user to reverse the draw order of this widget container
         */
        void ReverseDrawOrder(bool reversed = true);

        /**
         * Create a widget
         */
        template<typename T, typename... Args>
        T& createWidget(Args&&... p_args)
        {
            m_widgets.emplace_back(new T(p_args...), EMemoryMode::INTERNAL_MANAGMENT);
            T& instance = *reinterpret_cast<T*>(m_widgets.back().first);
            instance.SetParent(this);
            return instance;
        }

        /**
         * Returns the widgets and their memory management mode
         */
        std::vector<std::pair<Widgets::AWidget*, EMemoryMode>>& GetWidgets();

    protected:
        std::vector<std::pair<Widgets::AWidget*, EMemoryMode>> m_widgets;
        bool                                                   m_reversedDrawOrder = false;
    };
} // namespace LunarYue::UI::Internal