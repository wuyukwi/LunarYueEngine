#include "editor/include/editor_view.h"

#include "editor/include/editor_actions.h"
#include "function/ui/Widgets/Visual/Image.h"

namespace LunarYue
{

    AView::AView(const std::string& p_title, bool p_opened, const UI::Settings::PanelWindowSettings& p_windowSettings) :
        PanelWindow(p_title, p_opened, p_windowSettings)
    {
        m_cameraPosition = {-10.0f, 3.0f, 10.0f};
        m_cameraRotation = Quaternion(0.0f, 135.0f, 0.0f, 1.0f);

        //  m_image = &createWidget<UI::Widgets::Visual::Image>(m_fbo.GetTextureID(), Vector2 {0.f, 0.f});

        scrollable = false;
    }

    void AView::Update(float p_deltaTime)
    {
        auto [winWidth, winHeight] = GetSafeSize();

        m_image->size = Vector2(static_cast<float>(winWidth), static_cast<float>(winHeight));

        // m_fbo.Resize(winWidth, winHeight);
    }

    void AView::_Draw_Impl()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

        UI::Panels::PanelWindow::_Draw_Impl();

        ImGui::PopStyleVar();
    }

    void AView::Render()
    {
        FillEngineUBO();

        auto [winWidth, winHeight] = GetSafeSize();

        // EDITOR_CONTEXT(shapeDrawer)->SetViewProjection(m_camera.GetProjectionMatrix() * m_camera.GetViewMatrix());

        // EDITOR_CONTEXT(renderer)->SetViewPort(0, 0, winWidth, winHeight);

        _Render_Impl();
    }

    void AView::SetCameraPosition(const Vector3& p_position) { m_cameraPosition = p_position; }

    void AView::SetCameraRotation(const Quaternion& p_rotation) { m_cameraRotation = p_rotation; }

    const Vector3& AView::GetCameraPosition() const { return m_cameraPosition; }

    const Quaternion& AView::GetCameraRotation() const { return m_cameraRotation; }

    // OvRendering::LowRenderer::Camera& AView::GetCamera() { return m_camera; }

    std::pair<uint16_t, uint16_t> AView::GetSafeSize() const
    {
        auto result = GetSize() - Vector2 {0.f, 25.f}; // 25 == title bar height
        return {static_cast<uint16_t>(result.x), static_cast<uint16_t>(result.y)};
    }

    const Vector3& AView::GetGridColor() const { return m_gridColor; }

    void AView::SetGridColor(const Vector3& p_color) { m_gridColor = p_color; }

    void AView::FillEngineUBO()
    {
        // auto& engineUBO = *EDITOR_CONTEXT(engineUBO);

        // auto [winWidth, winHeight] = GetSafeSize();

        // size_t offset = sizeof(Matrix4); // We skip the model matrix (Which is a special case, modified every draw calls)
        // engineUBO.SetSubData(Matrix4::Transpose(m_camera.GetViewMatrix()), std::ref(offset));
        // engineUBO.SetSubData(Matrix4::Transpose(m_camera.GetProjectionMatrix()), std::ref(offset));
        // engineUBO.SetSubData(m_cameraPosition, std::ref(offset));
    }

    void AView::PrepareCamera()
    {
        // auto [winWidth, winHeight] = GetSafeSize();
        // m_camera.CacheMatrices(winWidth, winHeight, m_cameraPosition, m_cameraRotation);
    }
} // namespace LunarYue
