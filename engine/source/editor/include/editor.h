#pragma once

#include "runtime/core/math/vector2.h"

#include <memory>

namespace LunarYue
{
    class EditorLauncher;
    class EditorUI;
    class LunarYueEngine;

    class LunarYueEditor
    {
        friend class EditorLauncher;
        friend class EditorUI;

    public:
        LunarYueEditor();

        virtual ~LunarYueEditor();

        void initialize(std::shared_ptr<LunarYueEngine> engine_runtime);

        void clear();

        void run() const;

    protected:
        std::shared_ptr<EditorLauncher> m_editor_launcher;
        std::shared_ptr<EditorUI> m_editor_ui;

        std::shared_ptr<LunarYueEngine> m_engine_runtime;
    };
} // namespace LunarYue
