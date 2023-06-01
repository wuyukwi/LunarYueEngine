#pragma once
#include "editor_view_controllable.h"

namespace LunarYue
{
    class SceneView : public AView
    {
    public:
        /**
         * Constructor
         */
        SceneView(const std::string& p_title, bool p_opened, const UI::Settings::PanelWindowSettings& p_windowSettings);

        /**
         * Update the scene view
         */
        virtual void Update(float p_deltaTime);

        /**
         * Custom implementation of the render method
         */
        virtual void _Render_Impl() override;

        /**
         * Render the actual scene
         */
        void RenderScene(uint8_t p_defaultRenderState);

        /**
         * Render the scene for actor picking (Using unlit colors)
         */
        void RenderSceneForActorPicking();

        /**
         * Render the scene for actor picking and handle the logic behind it
         */
        void HandleActorPicking();

    private:
        // OvCore::SceneSystem::SceneManager& m_sceneManager;
        // OvRendering::Buffers::Framebuffer  m_actorPickingFramebuffer;
        // OvEditor::Core::GizmoBehaviour     m_gizmoOperations;
        // OvEditor::Core::EGizmoOperation    m_currentOperation = OvEditor::Core::EGizmoOperation::TRANSLATE;

        // std::optional<std::reference_wrapper<OvCore::ECS::Actor>> m_highlightedActor;
        // std::optional<OvEditor::Core::GizmoBehaviour::EDirection> m_highlightedGizmoDirection;
    };

} // namespace LunarYue