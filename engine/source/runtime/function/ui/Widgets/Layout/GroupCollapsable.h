/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <vector>

#include "function/ui/Event/Event.h"

#include "function/ui/Widgets/Layout/Group.h"

namespace LunarYue::UI::Widgets::Layout
{
	/**
	* Widget that can contains other widgets and is collapsable
	*/
	class GroupCollapsable : public Group
	{
	public:
		/**
		* Constructor
		* @param p_name
		*/
		GroupCollapsable(const std::string& p_name = "");

	protected:
		virtual void _Draw_Impl() override;

	public:
		std::string name;
		bool closable = false;
		bool opened = true;
		LunarYue::Eventing::Event<> CloseEvent;
		LunarYue::Eventing::Event<> OpenEvent;
	};
}