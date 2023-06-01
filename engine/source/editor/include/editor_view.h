#pragma once
#include "function/ui/Panels/PanelWindow.h"
#include "function/ui/Widgets/Visual/Image.h"
#include "runtime/core/math/vector3.h"

namespace LunarYue
{
    /**
     * Base class for any view
     */
    class AView : public UI::Panels::PanelWindow
    {
    public:
        /**
         * Constructor
         */
        AView(const std::string& p_title, bool p_opened, const UI::Settings::PanelWindowSettings& p_windowSettings);

        /**
         * Update the view
         */
        virtual void Update(float p_deltaTime);

        /**
         * Custom implementation of the draw method
         */
        void _Draw_Impl() override;

        /**
         * Custom implementation of the render method to define in dervied classes
         */
        virtual void _Render_Impl() = 0;

        /**
         * Render the view
         */
        void Render();

        /**
         * Defines the camera position
         */
        void SetCameraPosition(const Vector3& p_position);

        /**
         * Defines the camera rotation
         */
        void SetCameraRotation(const Quaternion& p_rotation);

        /**
         * Returns the camera position
         */
        const Vector3& GetCameraPosition() const;

        /**
         * Returns the camera rotation
         */
        const Quaternion& GetCameraRotation() const;

        /**
         * Returns the camera used by this view
         */
        // OvRendering::LowRenderer::Camera& GetCamera();

        /**
         * Returns the size of the panel ignoring its titlebar height
         */
        std::pair<uint16_t, uint16_t> GetSafeSize() const;

        /**
         * Returns the grid color of the view
         */
        const Vector3& GetGridColor() const;

        /**
         * Defines the grid color of the view
         * @param p_color
         */
        void SetGridColor(const Vector3& p_color);

        /**
         * Fill the UBO using the view settings
         */
        void FillEngineUBO();

    protected:
        /**
         * Update camera matrices
         */
        void PrepareCamera();

    protected:
        // OvEditor::Core::EditorRenderer&  m_editorRenderer;
        // OvRendering::LowRenderer::Camera m_camera {};
        Vector3                     m_cameraPosition;
        Quaternion                  m_cameraRotation;
        UI::Widgets::Visual::Image* m_image {};

        Vector3 m_gridColor = Vector3 {0.176f, 0.176f, 0.176f};

        // OvRendering::Buffers::Framebuffer m_fbo {};
    };
} // namespace LunarYue
