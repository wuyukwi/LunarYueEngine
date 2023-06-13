#pragma once

#include <array>

#include "function/ui/Event/Event.h"

#include "function/ui/Widgets/DataWidget.h"

namespace LunarYue::UI::Widgets::Drags
{
    /**
     * Drag widget of multiple generic type
     */
    template<typename T, size_t _Size>
    class DragMultipleScalars : public DataWidget<std::array<T, _Size>>
    {
        static_assert(_Size > 1, "Invalid DragMultipleScalars _Size (2 or more requiered)");

    public:
        /**
         * Constructor
         * @param p_dataType
         * @param p_min
         * @param p_max
         * @param p_value
         * @param p_speed
         * @param p_label
         * @param p_format
         */
        DragMultipleScalars(ImGuiDataType_     p_dataType,
                            T                  p_min,
                            T                  p_max,
                            T                  p_value,
                            float              p_speed,
                            const std::string& p_label,
                            const std::string& p_format) :
            DataWidget<std::array<T, _Size>>(values),
            m_dataType(p_dataType), min(p_min), max(p_max), speed(p_speed), label(p_label), format(p_format)
        {
            values.fill(p_value);
        }

    protected:
        void _Draw_Impl() override
        {
            if (max < min)
                max = min;

            for (size_t i = 0; i < _Size; ++i)
            {
                if (values[i] < min)
                    values[i] = min;
                else if (values[i] > max)
                    values[i] = max;
            }

            if (ImGui::DragScalarN((label + this->m_widgetID).c_str(), m_dataType, values.data(), _Size, speed, &min, &max, format.c_str()))
            {
                ValueChangedEvent.Invoke(values);
                this->NotifyChange();
            }
        }

    public:
        T                                                min;
        T                                                max;
        float                                            speed;
        std::array<T, _Size>                             values;
        std::string                                      label;
        std::string                                      format;
        LunarYue::Eventing::Event<std::array<T, _Size>&> ValueChangedEvent;

    protected:
        ImGuiDataType_ m_dataType;
    };
} // namespace LunarYue::UI::Widgets::Drags