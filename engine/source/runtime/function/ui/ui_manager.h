#pragma once

#include <string>
#include <unordered_map>

#include "bgfx/bgfx.h"

#include "entry/entry.h"

#include "runtime/function/ui/Modules/Canvas.h"

namespace LunarYue
{}
namespace LunarYue
{

    enum class EditorStyle
    {
        IM_CLASSIC_STYLE,
        IM_DARK_STYLE,
        IM_LIGHT_STYLE,
        DUNE_DARK, // https://www.unknowncheats.me/forum/direct3d/189635-imgui-style-settings.html
        ALTERNATIVE_DARK,
        PICCOLO_STYLE
    };

    class UIManager
    {
    public:
        UIManager();

        ~UIManager();

        void initialize(EditorStyle style = EditorStyle::IM_DARK_STYLE);

        /**
         * Apply a new style to the UI elements
         */
        void ApplyStyle(EditorStyle p_style);

        /**
         * Load a font (Returns true on success)
         */
        bool LoadFont(const std::string& p_id, const std::string& p_path, float p_fontSize);

        /**
         * Unload a font (Returns true on success)
         */
        bool UnloadFont(const std::string& p_id);

        /**
         * Set the given font as the current one (Returns true on success)
         */
        bool UseFont(const std::string& p_id);

        /**
         * Use the default font (ImGui default font)
         */
        void UseDefaultFont();

        /**
         * Allow the user to enable/disable .ini generation to save his editor layout
         */
        void EnableEditorLayoutSave(bool p_value) const;

        /**
         *  Return true if the editor layout save system is on
         */
        bool IsEditorLayoutSaveEnabled() const;

        /**
         * Defines a filename for the editor layout save file
         */
        void SetEditorLayoutSaveFilename(const std::string& p_filename);

        /**
         * Defines a frequency (in seconds) for the auto saving system of the editor layout
         */
        void SetEditorLayoutAutosaveFrequency(float p_frequency);

        /**
         * Returns the current frequency (in seconds) for the auto saving system of the editor layout
         */
        float GetEditorLayoutAutosaveFrequency(float p_frequeny);

        /**
         * Enable the docking system
         */
        void EnableDocking(bool p_value);

        /**
         * Reset the UI layout to the given configuration file
         */
        void ResetLayout(const std::string& p_config) const;

        /**
         * Return true if the docking system is enabled
         */
        bool IsDockingEnabled() const;

        /**
         * Defines the canvas to use
         */
        void SetCanvas(UI::Modules::Canvas& p_canvas);

        /**
         * Stop considering the current canvas (if any)
         */
        void RemoveCanvas();

        /**
         * Render ImGui current frane
         * @note Should be called once per frame
         */
        void Render();

    private:
        void PushCurrentFont();
        void PopCurrentFont();

    private:
        bool                                     m_dockingState {};
        UI::Modules::Canvas*                     m_currentCanvas = nullptr;
        std::unordered_map<std::string, ImFont*> m_fonts;
        std::string                              m_layoutSaveFilename = "imgui.ini";

        ImGuiContext*       m_imgui;
        bx::AllocatorI*     m_allocator;
        bgfx::VertexLayout  m_layout;
        bgfx::ProgramHandle m_program;
        bgfx::ProgramHandle m_imageProgram;
        bgfx::TextureHandle m_texture;
        bgfx::UniformHandle s_tex;
        bgfx::UniformHandle u_imageLodEnabled;
        ImFont*             m_font[ImGui::Font::Count];
        int64_t             m_last;
        int32_t             m_lastScroll;
        bgfx::ViewId        m_viewId;

        ImGuiKey m_keyMap[(int)entry::Key::Count];
    };
} // namespace LunarYue