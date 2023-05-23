/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <string>

#include "runtime/core/math/vector2.h"

#include "runtime/function/ui/Types/Color.h"
#include "function/ui/Widgets/Buttons/AButton.h"

namespace LunarYue::UI::Widgets::Buttons
{
	/**
	* Button widget of a single color (Color palette element)
	*/
	class ButtonColored : public AButton
	{
	public:
		/**
		* Constructor
		* @param p_label
		* @param p_color
		* @param p_size
		* @param p_enableAlpha
		*/
		ButtonColored(const std::string& p_label = "", const Types::Color& p_color = {}, const Vector2& p_size =Vector2(0.f, 0.f), bool p_enableAlpha = true);

	protected:
		void _Draw_Impl() override;

	public:
		std::string label;
		Types::Color color;
		Vector2 size;
		bool enableAlpha;
	};
}