/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include "runtime/function/ui/Panels/APanelTransformable.h"

namespace LunarYue::UI::Panels
{
	/**
	* A simple panel that is transformable and without decorations (No background)
	*/
	class PanelUndecorated : public APanelTransformable
	{
	public:
		void _Draw_Impl() override;

	private:
		int CollectFlags();
	};
}