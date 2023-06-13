#pragma once

#include <array>

#include "runtime/function/ui/Internal/WidgetContainer.h"

namespace LunarYue::UI::Widgets::Layout
{
    /**
     * Widget that allow columnification
     */
    template<size_t _Size>
    class Columns : public AWidget, public Internal::WidgetContainer
    {
    public:
        /**
         * Constructor
         */
        Columns() { widths.fill(-1.f); }

    protected:
        virtual void _Draw_Impl() override
        {
            ImGui::Columns(static_cast<int>(_Size), ("##" + m_widgetID).c_str(), false);

            int counter = 0;

            CollectGarbages();

            for (auto it = m_widgets.begin(); it != m_widgets.end();)
            {
                it->first->Draw();

                ++it;

                if (it != m_widgets.end())
                {
                    if (widths[counter] != -1.f)
                        ImGui::SetColumnWidth(counter, widths[counter]);

                    ImGui::NextColumn();
                }

                ++counter;

                if (counter == _Size)
                    counter = 0;
            }

            ImGui::Columns(1); // Necessary to not break the layout for following widget
        }

    public:
        std::array<float, _Size> widths;
    };
} // namespace LunarYue::UI::Widgets::Layout