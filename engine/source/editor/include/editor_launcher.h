#pragma once

#include <memory>

#include "bgfx_utils.h"

#include "entry/entry.h"
#include "entry/input.h"

#include "function/ui/Modules/Canvas.h"
#include "function/ui/Panels/PanelWindow.h"

namespace LunarYue
{

    class EditorLauncher : public entry::AppI
    {
    public:
        EditorLauncher(const char* _name        = "EditorLauncher",
                       const char* _description = "Author: HuangQiYue",
                       const char* _url         = "https://github.com/wuyukwi/LunarYueEngine");

        void init(int32_t _argc, const char* const* _argv, uint32_t _width, uint32_t _height) override;

        virtual int shutdown() override;

        bool update() override;

        void createWindow();

    protected:
        UI::Modules::Canvas                      m_canvas;
        std::unique_ptr<UI::Panels::PanelWindow> m_mainPanel;

        std::string m_projectPath;
        std::string m_projectName;
        bool        m_readyToGo = false;

        entry::MouseState m_mouseState;

        uint32_t m_width;
        uint32_t m_height;
        uint32_t m_debug;
        uint32_t m_reset;

        int64_t             m_timeOffset;
        Mesh*               m_mesh;
        bgfx::ProgramHandle m_program;
        bgfx::UniformHandle u_time;

        InputBinding* m_bindings;
    };
} // namespace LunarYue
