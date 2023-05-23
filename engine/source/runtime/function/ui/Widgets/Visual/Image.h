/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include "runtime/core/math/vector2.h"

#include "runtime/function/ui/Internal/TextureID.h"
#include "function/ui/Widgets/AWidget.h"

namespace LunarYue::UI::Widgets::Visual
{
	/**
	* Simple widget that display an image
	*/
	class Image : public AWidget
	{
	public:
		/**
		* Constructor
		* @param p_textureID
		* @parma p_size
		*/
		Image(uint32_t p_textureID, const Vector2& p_size);

	protected:
		void _Draw_Impl() override;

	public:
		Internal::TextureID textureID;
		Vector2 size;
	};
}