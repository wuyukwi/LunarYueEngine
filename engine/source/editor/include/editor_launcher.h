#pragma once

#include <memory>

#include "function/ui/Modules/Canvas.h"
#include "function/ui/Panels/PanelWindow.h"
#include "function/ui/window_ui.h"

namespace LunarYue
{
    class LunarYueEngine;

    class EditorLauncher : public WindowUI
    {
    public:
        EditorLauncher();

        void initialize() override;
        void preRender() override;

    protected:
        UI::Modules::Canvas                      m_canvas;
        std::unique_ptr<UI::Panels::PanelWindow> m_mainPanel;

        std::string m_projectPath;
        std::string m_projectName;
        bool        m_readyToGo = false;
    };
} // namespace LunarYue
