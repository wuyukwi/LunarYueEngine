#pragma once

#include <core/graphics/texture.h>

#include <runtime/assets/asset_handle.h>
#include <runtime/system/events.h>

#include <editor_core/gui/gui.h>

#include <map>
#include <memory>

class render_window;
//-----------------------------------------------------------------------------
// Main Class Declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Name : gui_system (Class)
/// <summary>
/// Class for the management of interface elements.
/// </summary>
//-----------------------------------------------------------------------------
struct gui_system
{
    gui_system();
    ~gui_system();

    void frame_begin(float);

    std::uint32_t get_draw_calls() const;

    void draw_begin(float dt);

    void draw_end();

    void save_layout(const std::string& filename = "default_filename");

    /// <summary>
    /// Load a layout from the specified file.
    /// </summary>
    /// <param name="filename">The name of the file to load. If not provided, the default filename is used ("default_filename").</param>
    /// <returns>True if the layout was successfully loaded; otherwise, false.</returns>
    bool load_layout(const std::string& filename = "default_filename");

private:
    void platform_events(const std::vector<SDL_Event>&);

    ImFontAtlas   atlas_;
    ImGuiContext* initial_context_ = nullptr;
    std::string   default_setting_file_ {"imgui.ini"};
};

namespace gui
{
    inline texture_info get_info(const std::shared_ptr<gfx::texture>& texture)
    {
        gui::texture_info info;
        info.texture      = texture;
        bool is_rt        = texture ? texture->is_render_target() : false;
        info.is_rt        = is_rt;
        info.is_origin_bl = gfx::is_origin_bottom_left();
        return info;
    }

    inline texture_info get_info(const asset_handle<gfx::texture>& asset) { return get_info(asset.get_asset()); }
} // namespace gui

struct gui_style
{
    struct hsv_setup
    {
        float col_main_hue = 0.0f;
        float col_main_sat = 0.78f;
        float col_main_val = 0.66f;

        float col_area_hue = 0.0f;
        float col_area_sat = 0.0f;
        float col_area_val = 0.7f;

        float col_back_hue = 0.0f;
        float col_back_sat = 0.0f;
        float col_back_val = 0.9;

        float col_text_hue   = 0.0f;
        float col_text_sat   = 0.0f;
        float col_text_val   = 0.0f;
        float frame_rounding = 10.0;
    };

    void             reset_style();
    void             set_style_colors(const hsv_setup& _setup);
    void             load_style();
    void             save_style();
    static hsv_setup get_dark_style();
    static hsv_setup get_lighter_red();
    hsv_setup        setup;
};

gui_style& get_gui_style();
