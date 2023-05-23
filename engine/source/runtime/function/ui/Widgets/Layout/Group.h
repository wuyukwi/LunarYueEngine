/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <vector>

#include "runtime/function/ui/Internal/WidgetContainer.h"

namespace LunarYue::UI::Widgets::Layout
{
	/**
	* Widget that can contains other widgets
	*/
	class Group : public AWidget, public Internal::WidgetContainer
	{
	protected:
		virtual void _Draw_Impl() override;
	};
}