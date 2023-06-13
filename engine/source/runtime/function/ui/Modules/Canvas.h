#pragma once
#include "runtime/function/ui/API/IDrawable.h"
#include "runtime/function/ui/Panels/APanel.h"

namespace LunarYue::UI::Modules
{
    /**
     * A Canvas represents the whole frame available for UI drawing
     */
    class Canvas : public API::IDrawable
    {
    public:
        /**
         * Adds a panel to the canvas
         * @param p_panel
         */
        void AddPanel(Panels::APanel& p_panel);

        /**
         * Removes a panel from the canvas
         * @param p_panel
         */
        void RemovePanel(Panels::APanel& p_panel);

        /**
         * Removes every panels from the canvas
         */
        void RemoveAllPanels();

        /**
         * Makes the canvas a dockspace (Dockable panels will be able to attach themselves to the canvas)
         */
        void MakeDockspace(bool p_state);

        /**
         * Returns true if the canvas is a dockspace
         */
        bool IsDockspace() const;

        /**
         * Draw the canvas (Draw every panels)
         */
        void Draw() override;

    private:
        std::vector<std::reference_wrapper<Panels::APanel>> m_panels;
        bool                                                m_isDockspace = false;
    };
} // namespace LunarYue::UI::Modules
