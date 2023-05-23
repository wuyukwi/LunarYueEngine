#pragma once

#include <unordered_map>

#include "editor/include/editor_menu_bar.h"
#include "function/ui/Modules/Canvas.h"
#include "function/ui/Panels/APanel.h"
#include "function/ui/Panels/PanelWindow.h"

namespace LunarYue
{
    /**
     * Handle the creation and storage of UI panels
     */
    class PanelsManager
    {
    public:
        /**
         * Constructor
         * @param p_canvas
         */
        PanelsManager(UI::Modules::Canvas& p_canvas) : m_canvas(p_canvas) {}

        /**
         * Create a panel
         * @param p_id
         * @param p_args
         */
        template<typename T, typename... Args>
        void CreatePanel(const std::string& p_id, Args&&... p_args)
        {
            if constexpr (std::is_base_of<UI::Panels::PanelWindow, T>::value)
            {
                m_panels.emplace(p_id, std::make_unique<T>(p_id, std::forward<Args>(p_args)...));
                T& instance = *static_cast<T*>(m_panels.at(p_id).get());
                GetPanelAs<MenuBar>("Menu Bar").RegisterPanel(instance.name, instance);
            }
            else
            {
                m_panels.emplace(p_id, std::make_unique<T>(std::forward<Args>(p_args)...));
            }

            m_canvas.AddPanel(*m_panels.at(p_id));
        }

        /**
         * Returns the panel identified by the given id casted in the given type
         * @param p_id
         */
        template<typename T>
        T& GetPanelAs(const std::string& p_id)
        {
            return *static_cast<T*>(m_panels[p_id].get());
        }

    private:
        std::unordered_map<std::string, std::unique_ptr<UI::Panels::APanel>> m_panels;
        UI::Modules::Canvas&                                                 m_canvas;
    };
} // namespace LunarYue
