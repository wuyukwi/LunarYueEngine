/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include "runtime/function/ui/Plugins/IPlugin.h"
#include "runtime/function/ui/Internal/WidgetContainer.h"
#include "function/ui/Widgets/Menu/MenuList.h"
#include "function/ui/Widgets/Menu/MenuItem.h"

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
}
