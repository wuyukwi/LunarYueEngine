/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include "function/ui/Widgets/AWidget.h"

namespace LunarYue::UI::Widgets::Visual
{
	/**
	* Simple widget that display a separator
	*/
	class Separator : public AWidget
	{
	protected:
		void _Draw_Impl() override;
	};
}