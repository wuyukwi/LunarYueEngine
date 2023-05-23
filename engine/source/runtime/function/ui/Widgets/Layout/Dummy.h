/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include "runtime/core/math/vector2.h"

#include "function/ui/Widgets/AWidget.h"

namespace LunarYue::UI::Widgets::Layout
{
	/**
	* Dummy widget that takes the given size as space in the panel
	*/
	class Dummy : public AWidget
	{
	public:
		/**
		* Constructor
		* @param p_size
		*/
		Dummy(const Vector2& p_size = { 0.0f, 0.0f });

	protected:
		void _Draw_Impl() override;

	public:
		Vector2 size;
	};
}