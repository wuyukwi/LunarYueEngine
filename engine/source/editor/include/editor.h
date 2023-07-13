#pragma once

#include "common.h"

#include <memory>

#include "editor_launcher.h"

namespace LunarYue
{
    class EditorLauncher;
    class EditorUI;
    class LunarYueEngine;

    class LunarYueEditor : public entry::AppI
    {

    public:
        LunarYueEditor(const char* _name        = "LunarYueEditor",
                       const char* _description = "Author: HuangQiYue",
                       const char* _url         = "https://github.com/wuyukwi/LunarYueEngine");

        void setupEngine(std::shared_ptr<LunarYueEngine> engine);

        void init(int32_t _argc, const char* const* _argv, uint32_t _width, uint32_t _height) override;

        virtual int shutdown() override;

        bool update() override;

    protected:
        std::shared_ptr<EditorLauncher> m_editor_launcher;
        std::shared_ptr<EditorUI>       m_editor_ui;

        std::shared_ptr<LunarYueEngine> m_engine_runtime;
    };
} // namespace LunarYue
