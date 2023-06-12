#include "editor//include/editor.h"

#include "runtime/engine.h"
#include "runtime/function/global/global_context.h"
#include "runtime/function/render/render_camera.h"
#include "runtime/function/render/render_system.h"

#include "editor/include/editor_global_context.h"
#include "editor/include/editor_input_manager.h"
#include "editor/include/editor_panels_manager.h"
#include "editor/include/editor_scene_manager.h"
#include "editor/include/editor_ui.h"
#include "function/render/window_system.h"
#include "function/ui/Core/UIManager.h"
#include "resource/asset_manager/asset_manager.h"

namespace LunarYue
{
    EditorGlobalContext g_editor_global_context;

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

    void LunarYueEditor::initialize(std::shared_ptr<LunarYueEngine> engine_runtime)
    {
        assert(engine_runtime);

        g_is_editor_mode = true;
        m_engine_runtime = engine_runtime;

        g_editor_global_context.m_scene_manager = std::make_shared<EditorSceneManager>();
        g_editor_global_context.m_input_manager = std::make_shared<EditorInputManager>();
        g_editor_global_context.m_scene_manager->initialize();
        g_editor_global_context.m_input_manager->initialize();

        g_editor_global_context.m_ui_manager = std::make_shared<UI::Core::UIManager>();
        g_editor_global_context.m_ui_manager->initialize(UI::Core::EditorStyle::DUNE_DARK);

        g_editor_global_context.m_engine_runtime = m_engine_runtime;
        g_editor_global_context.m_scene_manager->setEditorCamera(g_runtime_global_context.m_render_system->getRenderCamera());
        g_editor_global_context.m_scene_manager->uploadAxisResource();

        m_editor_ui = std::make_shared<EditorUI>();

        m_editor_ui->initialize();
    }

    void LunarYueEditor::clear() {}

    void LunarYueEditor::run() const
    {
        assert(m_engine_runtime);
        assert(m_editor_ui);

        while (true)
        {
            const float delta_time = m_engine_runtime->calculateDeltaTime();

            g_editor_global_context.m_scene_manager->tick(delta_time);
            g_editor_global_context.m_input_manager->tick(delta_time);

            if (!m_engine_runtime->tickOneFrame(delta_time))
                return;
        }
    }
} // namespace LunarYue
