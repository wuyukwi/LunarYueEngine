#pragma once
#include <memory>

#include "editor_ui.h"

namespace LunarYue
{

    class EditorGlobalContext
    {
    public:
        // editor
        std::shared_ptr<class EditorSceneManager>  m_scene_manager;
        std::shared_ptr<class EditorInputManager>  m_input_manager;
        std::shared_ptr<class UI::Core::UIManager> m_ui_manager;
        std::shared_ptr<class PanelsManager>       m_panels_manager;

        std::shared_ptr<class LunarYueEngine> m_engine_runtime;

        // engine
        std::shared_ptr<class AssetManager>      m_asset_manager;
        std::shared_ptr<class WindowSystem>      m_window_system;
        std::shared_ptr<class RenderSystem>      m_render_system;
        std::shared_ptr<class RenderDebugConfig> m_render_debug_config;
        std::shared_ptr<class ConfigManager>     m_config_manager;
        std::shared_ptr<class WorldManager>      m_world_manager;
    };

    extern EditorGlobalContext g_editor_global_context;

} // namespace LunarYue