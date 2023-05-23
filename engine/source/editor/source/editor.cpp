#include "editor//include/editor.h"

#include "runtime/engine.h"
#include "runtime/function/global/global_context.h"
#include "runtime/function/render/render_camera.h"
#include "runtime/function/render/render_system.h"

#include "editor/include/editor_global_context.h"
#include "editor/include/editor_input_manager.h"
#include "editor/include/editor_scene_manager.h"
#include "editor/include/editor_ui.h"
#include "function/render/window_system.h"
#include "resource/asset_manager/asset_manager.h"

namespace LunarYue
{
    void registerEdtorTickComponent(std::string component_type_name) { g_editor_tick_component_types.insert(component_type_name); }

    LunarYueEditor::LunarYueEditor()
    {
        registerEdtorTickComponent("TransformComponent");
        registerEdtorTickComponent("MeshComponent");
        // registerEdtorTickComponent("RigidbodyComponent");
        // registerEdtorTickComponent("MotorComponent");
        // registerEdtorTickComponent("AnimationComponent");
        // registerEdtorTickComponent("CameraComponent");
        // registerEdtorTickComponent("ParticleComponent");
        // registerEdtorTickComponent("LuaComponent");
    }

    LunarYueEditor::~LunarYueEditor() {}

    void LunarYueEditor::initialize(LunarYueEngine* engine_runtime)
    {
        assert(engine_runtime);

        g_is_editor_mode = true;
        m_engine_runtime = engine_runtime;

        EditorGlobalContextInitInfo init_info = {
            g_runtime_global_context.m_window_system.get(), g_runtime_global_context.m_render_system.get(), engine_runtime};
        g_editor_global_context.initialize(init_info);
        g_editor_global_context.m_scene_manager->setEditorCamera(g_runtime_global_context.m_render_system->getRenderCamera());
        g_editor_global_context.m_scene_manager->uploadAxisResource();

        m_editor_ui = std::make_shared<EditorUI>();

        WindowUIInitInfo ui_init_info = {
            g_runtime_global_context.m_window_system, g_runtime_global_context.m_render_system, g_runtime_global_context.m_config_manager};

        m_editor_ui->initialize(ui_init_info);
    }

    void LunarYueEditor::clear() { g_editor_global_context.clear(); }

    void LunarYueEditor::run()
    {
        assert(m_engine_runtime);
        assert(m_editor_ui);
        float delta_time;
        while (!g_runtime_global_context.m_window_system->shouldClose())
        {
            delta_time = m_engine_runtime->calculateDeltaTime();
            g_editor_global_context.m_scene_manager->tick(delta_time);
            g_editor_global_context.m_input_manager->tick(delta_time);
            if (!m_engine_runtime->tickOneFrame(delta_time))
                return;
        }
    }
} // namespace LunarYue
