/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#pragma once

#include "runtime/core/math/vector2.h"

#include "function/ui/Widgets/Plots/APlot.h"

namespace LunarYue::UI::Widgets::Plots
{
	/**
	* Plot displayed as an histogram
	*/
	class PlotHistogram : public APlot
	{
	public:
		/**
		* Constructor
		* @param p_data
		* @param p_minScale
		* @param p_maxScale
		* @param p_size
		* @param p_overlay
		* @param p_label
		* @param p_forceHover
		*/
		PlotHistogram
		(
			const std::vector<float>& p_data = std::vector<float>(),
			float p_minScale = std::numeric_limits<float>::min(),
			float p_maxScale = std::numeric_limits<float>::max(),
			const Vector2& p_size = { 0.0f, 0.0f },
			const std::string& p_overlay = "",
			const std::string& p_label = "",
			int p_forceHover = -1
		);

	protected:
		void _Draw_Impl() override;
	};
}