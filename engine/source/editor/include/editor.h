#pragma once

#include "common.h"

#include <memory>

namespace LunarYue
{
    class EditorUI;
    class LunarYueEngine;

    class LunarYueEditor : public entry::AppI
    {

    public:
        LunarYueEditor(const char* _name        = "LunarYueEditor",
                       const char* _description = "Author: HuangQiYue",
                       const char* _url         = "https://github.com/wuyukwi/LunarYueEngine");

        void init(int32_t _argc, const char* const* _argv, uint32_t _width, uint32_t _height) override;

        virtual int shutdown() override;

        bool update() override;

    protected:
        std::shared_ptr<EditorUI> m_editor_ui;

        std::shared_ptr<LunarYueEngine> m_engine_runtime;
    };
} // namespace LunarYue
