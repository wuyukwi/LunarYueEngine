#pragma once

#include "runtime/core/math/vector2.h"

#include <memory>

namespace LunarYue
{
    class EditorUI;
    class LunarYueEngine;

    class LunarYueEditor 
    {
        friend class EditorUI;

    public:
        LunarYueEditor();
        virtual ~LunarYueEditor();

        void initialize(LunarYueEngine* engine_runtime);
        void clear();

        void run();

    protected:
        std::shared_ptr<EditorUI> m_editor_ui;
        LunarYueEngine* m_engine_runtime{ nullptr };
    };
} // namespace LunarYue
