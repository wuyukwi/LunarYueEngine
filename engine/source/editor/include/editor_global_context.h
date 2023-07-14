#pragma once
#include <memory>

#include "editor_ui.h"

namespace LunarYue
{

    class EditorGlobalContext
    {
    public:
        // editor
        std::shared_ptr<class EditorSceneManager> m_scene_manager;
        std::shared_ptr<class EditorInputManager> m_input_manager;
        std::shared_ptr<class UIManager>          m_ui_manager;
        std::shared_ptr<class PanelsManager>      m_panels_manager;

        std::shared_ptr<class LunarYueEngine> m_engine_runtime;
    };

    extern EditorGlobalContext g_editor_global_context;

} // namespace LunarYue