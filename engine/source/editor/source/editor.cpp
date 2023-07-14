#include "editor/include/editor.h"

#include "runtime/engine.h"
#include "runtime/function/global/global_context.h"
#include "runtime/function/render/render_system.h"

#include "editor/include/editor_global_context.h"
#include "editor/include/editor_ui.h"

#include "function/render/window_system.h"

#include "imgui/imgui.h"

#include "resource/asset_manager/asset_manager.h"

#include "editor/include/editor_ui.h"

namespace LunarYue
{

    EditorGlobalContext g_editor_global_context;

    void registerEdtorTickComponent(const std::string& component_type_name) { g_editor_tick_component_types.insert(component_type_name); }

    LunarYueEditor::LunarYueEditor(const char* _name, const char* _description, const char* _url) : AppI(_name, _description, _url)
    {
        registerEdtorTickComponent("TransformComponent");
        registerEdtorTickComponent("MeshComponent");

        m_engine_runtime = std::make_shared<LunarYueEngine>();
        g_is_editor_mode = true;
    }

    void LunarYueEditor::setupEngine(std::shared_ptr<LunarYueEngine> engine)
    {
        assert(engine);
        // m_editor_launcher = std::make_shared<EditorLauncher>();
        // m_editor_launcher->initialize();

        // g_runtime_global_context.m_render_system->initializeUIRenderBackend(m_editor_ui);
    }

    void LunarYueEditor::init(int32_t _argc, const char* const* _argv, uint32_t _width, uint32_t _height)
    {
        // g_editor_global_context.m_scene_manager = std::make_shared<EditorSceneManager>();
        // g_editor_global_context.m_input_manager = std::make_shared<EditorInputManager>();
        // g_editor_global_context.m_scene_manager->initialize();
        // g_editor_global_context.m_input_manager->initialize();

        // g_editor_global_context.m_scene_manager->setEditorCamera(g_runtime_global_context.m_render_system->getRenderCamera());
        // g_editor_global_context.m_scene_manager->uploadAxisResource();

        const std::filesystem::path executable_path(_argv[0]);
        const std::filesystem::path config_file_path = executable_path.parent_path() / LunarYueEditorConfigFile;
        m_engine_runtime->startEngine(config_file_path.generic_string());

        m_engine_runtime->initialize();

        g_editor_global_context.m_ui_manager = std::make_shared<UIManager>();
        g_editor_global_context.m_ui_manager->initialize(EditorStyle::DUNE_DARK);

        m_editor_ui = std::make_shared<EditorUI>();
        m_editor_ui->initialize();
    }

    int LunarYueEditor::shutdown()
    {
        //        g_runtime_global_context.m_render_system->clear();
        m_engine_runtime->shutdownEngine();
        return 0;
    }

    bool LunarYueEditor::update()
    {
        const float delta_time = m_engine_runtime->calculateDeltaTime();

        while (!g_runtime_global_context.m_window_system->processWindowEvents())
        {

            g_runtime_global_context.m_render_system->beginFrame();

            m_engine_runtime->tickOneFrame(delta_time);

            m_editor_ui->preRender();

            g_runtime_global_context.m_render_system->endFrame();
        }

        return false;
    }
} // namespace LunarYue
