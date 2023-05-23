/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <vector>
#include <memory>
#include <algorithm>

#include "function/ui/Event/Event.h"

#include "runtime/function/ui/Panels/APanel.h"
#include "function/ui/Widgets/Menu/MenuList.h"

namespace LunarYue::UI::Panels
{
	/**
	* A simple panel that will be displayed on the top side of the canvas
	*/
	class PanelMenuBar : public APanel
	{
	protected:
		void _Draw_Impl() override;
	};
}