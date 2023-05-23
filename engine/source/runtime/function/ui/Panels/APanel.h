/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include <vector>
#include <unordered_map>

#include "runtime/function/ui/Internal/WidgetContainer.h"

namespace LunarYue::UI::Panels
{
	/**
	* A Panel is a component of a canvas. It is a sort of window in the UI
	*/
	class APanel : public API::IDrawable, public Internal::WidgetContainer
	{
	public:
		/**
		* Constructor
		*/
		APanel();

		/**
		* Draw the panel
		*/
		void Draw() override;

		/**
		* Returns the panel identifier
		*/
		const std::string& GetPanelID() const;

	protected:
		virtual void _Draw_Impl() = 0;

	public:
		bool enabled = true;

	protected:
		std::string m_panelID;

	private:
		static uint64_t __PANEL_ID_INCREMENT;
	};
}