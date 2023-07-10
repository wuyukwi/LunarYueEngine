#pragma once

#include <string>

#include "dear-imgui/imgui.h"
#include "runtime/function/ui/API/IDrawable.h"
#include "runtime/function/ui/Plugins/DataDispatcher.h"
#include "runtime/function/ui/Plugins/Pluginable.h"

namespace LunarYue::UI::Internal
{
    class WidgetContainer;
}

namespace LunarYue::UI::Widgets
{
    /**
     * A AWidget is the base class for any widget of runtime/function/ui/.
     * It is basically a visual element that can be placed into a panel.
     * It is drawable and can receive plugins
     */
    class AWidget : public API::IDrawable, public Plugins::Pluginable
    {
    public:
        /**
         * Constructor
         */
        AWidget();

        /**
         * Draw the widget on the panel
         */
        virtual void Draw() override;

        /**
         * Link the widget to another one by making its id identical to the given widget
         * @param p_widget
         */
        void LinkTo(const AWidget& p_widget);

        /**
         * Mark the widget as destroyed. It will be removed from the owner panel
         * by the garbage collector
         */
        void Destroy();

        /**
         * Returns true if the widget is marked as destroyed
         */
        bool IsDestroyed() const;

        /**
         * Defines the parent of this widget
         * @param p_parent
         */
        void SetParent(Internal::WidgetContainer* p_parent);

        /**
         * Returns true if the widget has a parent
         */
        bool HasParent() const;

        /**
         * Returns the parent of the widget or nullptr if there is no parent
         */
        Internal::WidgetContainer* GetParent();

    protected:
        virtual void _Draw_Impl() = 0;

    public:
        bool enabled   = true;
        bool lineBreak = true;

    protected:
        Internal::WidgetContainer* m_parent {};
        std::string                m_widgetID           = "?";
        bool                       m_autoExecutePlugins = true;

    private:
        static uint64_t __WIDGET_ID_INCREMENT;
        bool            m_destroyed = false;
    };
} // namespace LunarYue::UI::Widgets