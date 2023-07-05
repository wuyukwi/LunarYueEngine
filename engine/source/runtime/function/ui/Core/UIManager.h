#pragma once

#include <string>
#include <unordered_map>

#include "runtime/function/ui/Modules/Canvas.h"

namespace LunarYue
{
    class WindowUIInitInfo;
}
namespace LunarYue::UI::Core
{
    /**
     * Some styles that you can use to modify the UI looks
     */
    enum class EditorStyle
    {
        IM_CLASSIC_STYLE,
        IM_DARK_STYLE,
        IM_LIGHT_STYLE,
        DUNE_DARK, // https://www.unknowncheats.me/forum/direct3d/189635-imgui-style-settings.html
        ALTERNATIVE_DARK,
        PICCOLO_STYLE
    };

    /**
     * Handle the creation and drawing of the UI
     */
    class UIManager
    {
    public:
        UIManager();

        /**
         * Destroy the UI manager. Will handle ImGui destruction internally
         */
        ~UIManager();

        void initialize(EditorStyle p_style = EditorStyle::IM_DARK_STYLE);

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
        void SetCanvas(Modules::Canvas& p_canvas);

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
        Modules::Canvas*                         m_currentCanvas = nullptr;
        std::unordered_map<std::string, ImFont*> m_fonts;
        std::string                              m_layoutSaveFilename = "imgui.ini";
    };
} // namespace LunarYue::UI::Core