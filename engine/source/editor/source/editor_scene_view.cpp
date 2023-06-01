#include "editor/include/editor_scene_view.h"

#include <imgui_internal.h>

#include "editor/include/editor_global_context.h"
#include "editor/include/editor_input_manager.h"

#include "function/global/global_context.h"
#include "function/render/render_system.h"
#include "function/render/window_system.h"

namespace LunarYue
{

    SceneView::SceneView(const std::string& p_title, bool p_opened, const UI::Settings::PanelWindowSettings& p_windowSettings) :
        AView(p_title, p_opened, p_windowSettings)
    {
        /*    m_camera.SetClearColor({0.098f, 0.098f, 0.098f});
            m_camera.SetFar(5000.0f);

            m_image->AddPlugin<OvUI::Plugins::DDTarget<std::pair<std::string, OvUI::Widgets::Layout::Group*>>>("File").DataReceivedEvent +=
                [this](auto p_data) {
                    std::string path = p_data.first;

                    switch (OvTools::Utils::PathParser::GetFileType(path))
                    {
                        case OvTools::Utils::PathParser::EFileType::SCENE:
                            EDITOR_EXEC(LoadSceneFromDisk(path));
                            break;
                        case OvTools::Utils::PathParser::EFileType::MODEL:
                            EDITOR_EXEC(CreateActorWithModel(path, true));
                            break;
                    }
                };*/
        scrollable = false;
    }

    void SceneView::Update(float p_deltaTime) {}

    void SceneView::_Render_Impl()
    {
        hideBackground = true;

        // auto [winWidth, winHeight] = GetSafeSize();
        //  coord of right bottom point of full window minus coord of right bottom point of menu bar window.

        // if (new_window_pos != m_engine_window_pos || new_window_size != m_engine_window_size)
        {
#if defined(__MACH__)
            // The dpi_scale is not reactive to DPI changes or monitor switching, it might be a bug from ImGui.
            // Return value from ImGui::GetMainViewport()->DpiScal is always the same as first frame.
            // glfwGetMonitorContentScale and glfwSetWindowContentScaleCallback are more adaptive.
            float dpi_scale = main_viewport->DpiScale;
            g_runtime_global_context.m_render_system->updateEngineContentViewport(render_target_window_pos.x * dpi_scale,
                                                                                  render_target_window_pos.y * dpi_scale,
                                                                                  render_target_window_size.x * dpi_scale,
                                                                                  render_target_window_size.y * dpi_scale);
#else
            g_runtime_global_context.m_render_system->updateEngineContentViewport(m_position.x, m_position.y, m_size.x, m_size.y);
#endif
            g_editor_global_context.m_input_manager->setEngineWindowPos(m_position);
            g_editor_global_context.m_input_manager->setEngineWindowSize(m_size);
        }
    }

    void SceneView::RenderScene(uint8_t p_defaultRenderState) {}

    void SceneView::RenderSceneForActorPicking() {}

    bool IsResizing()
    {
        auto cursor = ImGui::GetMouseCursor();

        return cursor == ImGuiMouseCursor_ResizeEW || cursor == ImGuiMouseCursor_ResizeNS || cursor == ImGuiMouseCursor_ResizeNWSE ||
               cursor == ImGuiMouseCursor_ResizeNESW || cursor == ImGuiMouseCursor_ResizeAll;
        ;
    }

    void SceneView::HandleActorPicking()
    {
        // using namespace OvWindowing::Inputs;

        // auto& inputManager = *EDITOR_CONTEXT(inputManager);

        // if (inputManager.IsMouseButtonReleased(EMouseButton::MOUSE_BUTTON_LEFT))
        //{
        //     m_gizmoOperations.StopPicking();
        // }

        // if (IsHovered() && !IsResizing())
        //{
        //     RenderSceneForActorPicking();

        //    // Look actor under mouse
        //    auto [mouseX, mouseY] = inputManager.GetMousePosition();
        //    mouseX -= m_position.x;
        //    mouseY -= m_position.y;
        //    mouseY = GetSafeSize().second - mouseY + 25;

        //    m_actorPickingFramebuffer.Bind();
        //    uint8_t pixel[3];
        //    EDITOR_CONTEXT(renderer)->ReadPixels(static_cast<int>(mouseX),
        //                                         static_cast<int>(mouseY),
        //                                         1,
        //                                         1,
        //                                         OvRendering::Settings::EPixelDataFormat::RGB,
        //                                         OvRendering::Settings::EPixelDataType::UNSIGNED_BYTE,
        //                                         pixel);
        //    m_actorPickingFramebuffer.Unbind();

        //    uint32_t actorID         = (0 << 24) | (pixel[2] << 16) | (pixel[1] << 8) | (pixel[0] << 0);
        //    auto     actorUnderMouse = EDITOR_CONTEXT(sceneManager).GetCurrentScene()->FindActorByID(actorID);
        //    auto     direction       = m_gizmoOperations.IsPicking() ? m_gizmoOperations.GetDirection() :
        //                               EDITOR_EXEC(IsAnyActorSelected()) && pixel[0] == 255 && pixel[1] == 255 && pixel[2] >= 252 && pixel[2] <= 254
        //                               ?
        //                                                               static_cast<OvEditor::Core::GizmoBehaviour::EDirection>(pixel[2] - 252) :
        //                                                               std::optional<Core::GizmoBehaviour::EDirection> {};

        //    m_highlightedActor          = {};
        //    m_highlightedGizmoDirection = {};

        //    if (!m_cameraController.IsRightMousePressed())
        //    {
        //        if (direction.has_value())
        //        {
        //            m_highlightedGizmoDirection = direction;
        //        }
        //        else if (actorUnderMouse != nullptr)
        //        {
        //            m_highlightedActor = std::ref(*actorUnderMouse);
        //        }
        //    }

        //    /* Click */
        //    if (inputManager.IsMouseButtonPressed(EMouseButton::MOUSE_BUTTON_LEFT) && !m_cameraController.IsRightMousePressed())
        //    {
        //        /* Gizmo picking */
        //        if (direction.has_value())
        //        {
        //            m_gizmoOperations.StartPicking(EDITOR_EXEC(GetSelectedActor()), m_cameraPosition, m_currentOperation, direction.value());
        //        }
        //        /* Actor picking */
        //        else
        //        {

        //            if (actorUnderMouse)
        //            {
        //                EDITOR_EXEC(SelectActor(*actorUnderMouse));
        //            }
        //            else
        //            {
        //                EDITOR_EXEC(UnselectActor());
        //            }
        //        }
        //    }
        //}

        // if (m_gizmoOperations.IsPicking())
        //{
        //     auto mousePosition = EDITOR_CONTEXT(inputManager)->GetMousePosition();

        //    auto [winWidth, winHeight] = GetSafeSize();

        //    m_gizmoOperations.SetCurrentMouse({static_cast<float>(mousePosition.first), static_cast<float>(mousePosition.second)});
        //    m_gizmoOperations.ApplyOperation(
        //        m_camera.GetViewMatrix(), m_camera.GetProjectionMatrix(), {static_cast<float>(winWidth), static_cast<float>(winHeight)});
        //}
    }
} // namespace LunarYue
