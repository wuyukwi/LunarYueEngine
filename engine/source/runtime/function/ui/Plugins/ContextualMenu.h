#pragma once

#include "runtime/function/ui/Internal/WidgetContainer.h"
#include "runtime/function/ui/Plugins/IPlugin.h"

namespace LunarYue::UI::Plugins
{
    /**
     * A simple plugin that will show a contextual menu on right click
     * You can add widgets to a contextual menu
     */
    class ContextualMenu : public IPlugin, public Internal::WidgetContainer
    {
    public:
        /**
         * Execute the plugin
         */
        void Execute();

        /**
         * Force close the contextual menu
         */
        void Close();
    };
} // namespace LunarYue::UI::Plugins
