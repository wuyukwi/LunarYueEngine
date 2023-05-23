/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include "function/ui/Event/Event.h"

#include "function/ui/Widgets/DataWidget.h"

namespace LunarYue::UI::Widgets::InputFields
{
	/**
	* Input widget of type string
	*/
	class InputText : public DataWidget<std::string>
	{
	public:
		/**
		* Constructor
		* @param p_content
		* @param p_label
		*/
		InputText(const std::string& p_content = "", const std::string& p_label = "");

	protected:
		void _Draw_Impl() override;

	public:
		std::string content;
		std::string label;
		bool selectAllOnClick = false;
		LunarYue::Eventing::Event<std::string> ContentChangedEvent;
		LunarYue::Eventing::Event<std::string> EnterPressedEvent;
	};
}