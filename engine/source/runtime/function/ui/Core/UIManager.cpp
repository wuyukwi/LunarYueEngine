#include "runtime/function/ui/Core/UIManager.h"

#include "GLFW/glfw3.h"
#include "editor/include/IconsFontAwesome6.h"
#include "editor/include/editor_global_context.h"
#include "function/render/window_system.h"
#include "resource/config_manager/config_manager.h"
#include "stb_image.h"

namespace LunarYue::UI::Core
{
    inline void windowContentScaleUpdate(float scale)
    {
#if defined(__GNUC__) && defined(__MACH__)
        float font_scale               = fmaxf(1.0f, scale);
        ImGui::GetIO().FontGlobalScale = 1.0f / font_scale;
#endif
        // TOOD: Reload fonts if DPI scale is larger than previous font loading DPI scale
    }

    inline void windowContentScaleCallback(GLFWwindow* window, float x_scale, float y_scale) { windowContentScaleUpdate(fmaxf(x_scale, y_scale)); }

    UIManager::UIManager() {}

    UIManager::~UIManager() {}

    void UIManager::initialize(EditorStyle p_style)
    {
        // create imgui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        // set ui content scale
        float x_scale, y_scale;
        glfwGetWindowContentScale(g_editor_global_context.m_window_system->getWindow(), &x_scale, &y_scale);
        float content_scale = fmaxf(1.0f, fmaxf(x_scale, y_scale));
        windowContentScaleUpdate(content_scale);
        glfwSetWindowContentScaleCallback(g_editor_global_context.m_window_system->getWindow(), windowContentScaleCallback);

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigDockingAlwaysTabBar         = true;
        io.ConfigWindowsMoveFromTitleBarOnly = true;

        // デフォルトフォントをロードするための設定
        ImFontConfig font_config;
        font_config.MergeMode = true; // Font Awesome アイコンをデフォルトフォントとマージ
        const float font_size = content_scale * 16;

        ImVector<ImWchar>        icon_ranges;
        ImFontGlyphRangesBuilder builder;
        builder.AddRanges(io.Fonts->GetGlyphRangesDefault()); // デフォルト範囲を追加
        static const ImWchar icon_ranges_fa[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
        builder.AddRanges(icon_ranges_fa);

        // 上記と同じ形式で必要なFont Awesomeアイコンを追加
        builder.BuildRanges(&icon_ranges); // 最終結果を構築

        // デフォルトフォントをロード
        m_fonts["default"] = io.Fonts->AddFontFromFileTTF(
            g_editor_global_context.m_config_manager->getEditorFontPath().generic_string().data(), font_size, nullptr, icon_ranges.Data);

        // Font Awesome フォントをロード
        std::filesystem::path fa_font_path = g_editor_global_context.m_config_manager->getEditorFontPath().parent_path() / "fa-solid-900.ttf";
        m_fonts["icon"] = io.Fonts->AddFontFromFileTTF(fa_font_path.generic_string().data(), font_size, &font_config, icon_ranges.Data);

        io.Fonts->Build();

        // set imgui color style
        ApplyStyle(p_style);

        // setup window icon
        GLFWimage   window_icon[2];
        std::string big_icon_path_string   = g_editor_global_context.m_config_manager->getEditorBigIconPath().generic_string();
        std::string small_icon_path_string = g_editor_global_context.m_config_manager->getEditorSmallIconPath().generic_string();
        window_icon[0].pixels              = stbi_load(big_icon_path_string.data(), &window_icon[0].width, &window_icon[0].height, nullptr, 4);
        window_icon[1].pixels              = stbi_load(small_icon_path_string.data(), &window_icon[1].width, &window_icon[1].height, nullptr, 4);
        glfwSetWindowIcon(g_editor_global_context.m_window_system->getWindow(), 2, window_icon);
        stbi_image_free(window_icon[0].pixels);
        stbi_image_free(window_icon[1].pixels);
    }

    void UIManager::ApplyStyle(EditorStyle p_style)
    {
        ImGuiStyle* style = &ImGui::GetStyle();

        switch (p_style)
        {
            case EditorStyle::IM_CLASSIC_STYLE:
                ImGui::StyleColorsClassic();
                break;
            case EditorStyle::IM_DARK_STYLE:
                ImGui::StyleColorsDark();
                break;
            case EditorStyle::IM_LIGHT_STYLE:
                ImGui::StyleColorsLight();
                break;
        }

        if (p_style == EditorStyle::DUNE_DARK)
        {
            style->WindowPadding     = ImVec2(15, 15);
            style->WindowRounding    = 5.0f;
            style->FramePadding      = ImVec2(5, 5);
            style->FrameRounding     = 4.0f;
            style->ItemSpacing       = ImVec2(12, 8);
            style->ItemInnerSpacing  = ImVec2(8, 6);
            style->IndentSpacing     = 25.0f;
            style->ScrollbarSize     = 15.0f;
            style->ScrollbarRounding = 9.0f;
            style->GrabMinSize       = 5.0f;
            style->GrabRounding      = 3.0f;

            style->Colors[ImGuiCol_Text]                 = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
            style->Colors[ImGuiCol_TextDisabled]         = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
            style->Colors[ImGuiCol_WindowBg]             = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
            style->Colors[ImGuiCol_ChildBg]              = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
            style->Colors[ImGuiCol_PopupBg]              = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
            style->Colors[ImGuiCol_Border]               = ImVec4(0.2f, 0.2f, 0.2f, 0.88f);
            style->Colors[ImGuiCol_BorderShadow]         = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
            style->Colors[ImGuiCol_FrameBg]              = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
            style->Colors[ImGuiCol_FrameBgHovered]       = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
            style->Colors[ImGuiCol_FrameBgActive]        = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
            style->Colors[ImGuiCol_TitleBg]              = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
            style->Colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(0.3f, 0.3f, 0.3f, 0.75f);
            style->Colors[ImGuiCol_TitleBgActive]        = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
            style->Colors[ImGuiCol_MenuBarBg]            = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
            style->Colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
            style->Colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
            style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
            style->Colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
            style->Colors[ImGuiCol_CheckMark]            = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
            style->Colors[ImGuiCol_SliderGrab]           = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
            style->Colors[ImGuiCol_SliderGrabActive]     = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
            style->Colors[ImGuiCol_Button]               = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
            style->Colors[ImGuiCol_ButtonHovered]        = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
            style->Colors[ImGuiCol_ButtonActive]         = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
            style->Colors[ImGuiCol_Header]               = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
            style->Colors[ImGuiCol_HeaderHovered]        = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
            style->Colors[ImGuiCol_HeaderActive]         = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
            style->Colors[ImGuiCol_Separator]            = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
            style->Colors[ImGuiCol_SeparatorHovered]     = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
            style->Colors[ImGuiCol_SeparatorActive]      = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
            style->Colors[ImGuiCol_ResizeGrip]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
            style->Colors[ImGuiCol_ResizeGripHovered]    = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
            style->Colors[ImGuiCol_ResizeGripActive]     = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
            style->Colors[ImGuiCol_PlotLines]            = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
            style->Colors[ImGuiCol_PlotLinesHovered]     = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
            style->Colors[ImGuiCol_PlotHistogram]        = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
            style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
            style->Colors[ImGuiCol_TextSelectedBg]       = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
            style->Colors[ImGuiCol_ModalWindowDimBg]     = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);

            style->Colors[ImGuiCol_Tab] = style->Colors[ImGuiCol_TabUnfocused];
        }
        else if (p_style == EditorStyle::ALTERNATIVE_DARK)
        {
            style->WindowPadding     = ImVec2(15, 15);
            style->WindowRounding    = 0.0f;
            style->FramePadding      = ImVec2(5, 5);
            style->FrameRounding     = 0.0f;
            style->ItemSpacing       = ImVec2(12, 8);
            style->ItemInnerSpacing  = ImVec2(8, 6);
            style->IndentSpacing     = 25.0f;
            style->ScrollbarSize     = 15.0f;
            style->ScrollbarRounding = 0.0f;
            style->GrabMinSize       = 5.0f;
            style->GrabRounding      = 0.0f;
            style->TabRounding       = 0.0f;
            style->ChildRounding     = 0.0f;
            style->PopupRounding     = 0.0f;

            style->WindowBorderSize = 1.0f;
            style->FrameBorderSize  = 0.0f;
            style->PopupBorderSize  = 1.0f;

            ImVec4* colors                         = ImGui::GetStyle().Colors;
            colors[ImGuiCol_Text]                  = ImVec4(0.96f, 0.96f, 0.99f, 1.00f);
            colors[ImGuiCol_TextDisabled]          = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
            colors[ImGuiCol_WindowBg]              = ImVec4(0.09f, 0.09f, 0.10f, 1.00f);
            colors[ImGuiCol_ChildBg]               = ImVec4(0.09f, 0.09f, 0.10f, 1.00f);
            colors[ImGuiCol_PopupBg]               = ImVec4(0.06f, 0.06f, 0.07f, 1.00f);
            colors[ImGuiCol_Border]                = ImVec4(0.12f, 0.12f, 0.14f, 1.00f);
            colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
            colors[ImGuiCol_FrameBg]               = ImVec4(0.12f, 0.12f, 0.13f, 1.00f);
            colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
            colors[ImGuiCol_FrameBgActive]         = ImVec4(0.27f, 0.27f, 0.29f, 1.00f);
            colors[ImGuiCol_TitleBg]               = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
            colors[ImGuiCol_TitleBgActive]         = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
            colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
            colors[ImGuiCol_MenuBarBg]             = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
            colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.07f, 0.07f, 0.07f, 1.00f);
            colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.31f, 0.31f, 0.32f, 1.00f);
            colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.41f, 0.41f, 0.42f, 1.00f);
            colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.51f, 0.51f, 0.53f, 1.00f);
            colors[ImGuiCol_CheckMark]             = ImVec4(0.44f, 0.44f, 0.47f, 1.00f);
            colors[ImGuiCol_SliderGrab]            = ImVec4(0.44f, 0.44f, 0.47f, 1.00f);
            colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.59f, 0.59f, 0.61f, 1.00f);
            colors[ImGuiCol_Button]                = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
            colors[ImGuiCol_ButtonHovered]         = ImVec4(0.44f, 0.44f, 0.47f, 1.00f);
            colors[ImGuiCol_ButtonActive]          = ImVec4(0.59f, 0.59f, 0.61f, 1.00f);
            colors[ImGuiCol_Header]                = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
            colors[ImGuiCol_HeaderHovered]         = ImVec4(0.44f, 0.44f, 0.47f, 1.00f);
            colors[ImGuiCol_HeaderActive]          = ImVec4(0.59f, 0.59f, 0.61f, 1.00f);
            colors[ImGuiCol_Separator]             = ImVec4(1.00f, 1.00f, 1.00f, 0.20f);
            colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.44f, 0.44f, 0.47f, 0.39f);
            colors[ImGuiCol_SeparatorActive]       = ImVec4(0.44f, 0.44f, 0.47f, 0.59f);
            colors[ImGuiCol_ResizeGrip]            = ImVec4(0.26f, 0.59f, 0.98f, 0.00f);
            colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.26f, 0.59f, 0.98f, 0.00f);
            colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.26f, 0.59f, 0.98f, 0.00f);
            colors[ImGuiCol_Tab]                   = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
            colors[ImGuiCol_TabHovered]            = ImVec4(0.44f, 0.44f, 0.47f, 1.00f);
            colors[ImGuiCol_TabActive]             = ImVec4(0.44f, 0.44f, 0.47f, 1.00f);
            colors[ImGuiCol_TabUnfocused]          = ImVec4(0.20f, 0.20f, 0.22f, 0.39f);
            colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.44f, 0.44f, 0.47f, 0.39f);
            colors[ImGuiCol_DockingPreview]        = ImVec4(0.91f, 0.62f, 0.00f, 0.78f);
            colors[ImGuiCol_DockingEmptyBg]        = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
            colors[ImGuiCol_PlotLines]             = ImVec4(0.96f, 0.96f, 0.99f, 1.00f);
            colors[ImGuiCol_PlotLinesHovered]      = ImVec4(0.12f, 1.00f, 0.12f, 1.00f);
            colors[ImGuiCol_PlotHistogram]         = ImVec4(0.96f, 0.96f, 0.99f, 1.00f);
            colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(0.12f, 1.00f, 0.12f, 1.00f);
            colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
            colors[ImGuiCol_DragDropTarget]        = ImVec4(0.91f, 0.62f, 0.00f, 1.00f);
            colors[ImGuiCol_NavHighlight]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
            colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
            colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
            colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
        }
        else if (p_style == EditorStyle::PICCOLO_STYLE)
        {
            style->WindowPadding   = ImVec2(1.0, 0);
            style->FramePadding    = ImVec2(14.0, 2.0f);
            style->ChildBorderSize = 0.0f;
            style->FrameRounding   = 5.0f;
            style->FrameBorderSize = 1.5f;

            ImVec4* colors                         = ImGui::GetStyle().Colors;
            colors[ImGuiCol_Text]                  = ImVec4(0.4745f, 0.4745f, 0.4745f, 1.00f);
            colors[ImGuiCol_TextDisabled]          = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
            colors[ImGuiCol_WindowBg]              = ImVec4(0.0078f, 0.0078f, 0.0078f, 1.00f);
            colors[ImGuiCol_ChildBg]               = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
            colors[ImGuiCol_PopupBg]               = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
            colors[ImGuiCol_Border]                = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
            colors[ImGuiCol_BorderShadow]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
            colors[ImGuiCol_FrameBg]               = ImVec4(0.047f, 0.047f, 0.047f, 0.5411f);
            colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.196f, 0.196f, 0.196f, 0.40f);
            colors[ImGuiCol_FrameBgActive]         = ImVec4(0.294f, 0.294f, 0.294f, 0.67f);
            colors[ImGuiCol_TitleBg]               = ImVec4(0.0039f, 0.0039f, 0.0039f, 1.00f);
            colors[ImGuiCol_TitleBgActive]         = ImVec4(0.0039f, 0.0039f, 0.0039f, 1.00f);
            colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.00f, 0.00f, 0.00f, 0.50f);
            colors[ImGuiCol_MenuBarBg]             = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
            colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
            colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
            colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
            colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
            colors[ImGuiCol_CheckMark]             = ImVec4(93.0f / 255.0f, 10.0f / 255.0f, 66.0f / 255.0f, 1.00f);
            colors[ImGuiCol_SliderGrab]            = colors[ImGuiCol_CheckMark];
            colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.3647f, 0.0392f, 0.2588f, 0.50f);
            colors[ImGuiCol_Button]                = ImVec4(0.0117f, 0.0117f, 0.0117f, 1.00f);
            colors[ImGuiCol_ButtonHovered]         = ImVec4(0.0235f, 0.0235f, 0.0235f, 1.00f);
            colors[ImGuiCol_ButtonActive]          = ImVec4(0.0353f, 0.0196f, 0.0235f, 1.00f);
            colors[ImGuiCol_Header]                = ImVec4(0.1137f, 0.0235f, 0.0745f, 0.588f);
            colors[ImGuiCol_HeaderHovered]         = ImVec4(5.0f / 255.0f, 5.0f / 255.0f, 5.0f / 255.0f, 1.00f);
            colors[ImGuiCol_HeaderActive]          = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
            colors[ImGuiCol_Separator]             = ImVec4(0.0f, 0.0f, 0.0f, 0.50f);
            colors[ImGuiCol_SeparatorHovered]      = ImVec4(45.0f / 255.0f, 7.0f / 255.0f, 26.0f / 255.0f, 1.00f);
            colors[ImGuiCol_SeparatorActive]       = ImVec4(45.0f / 255.0f, 7.0f / 255.0f, 26.0f / 255.0f, 1.00f);
            colors[ImGuiCol_ResizeGrip]            = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
            colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
            colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
            colors[ImGuiCol_Tab]                   = ImVec4(6.0f / 255.0f, 6.0f / 255.0f, 8.0f / 255.0f, 1.00f);
            colors[ImGuiCol_TabHovered]            = ImVec4(45.0f / 255.0f, 7.0f / 255.0f, 26.0f / 255.0f, 150.0f / 255.0f);
            colors[ImGuiCol_TabActive]             = ImVec4(47.0f / 255.0f, 6.0f / 255.0f, 29.0f / 255.0f, 1.0f);
            colors[ImGuiCol_TabUnfocused]          = ImVec4(45.0f / 255.0f, 7.0f / 255.0f, 26.0f / 255.0f, 25.0f / 255.0f);
            colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(6.0f / 255.0f, 6.0f / 255.0f, 8.0f / 255.0f, 200.0f / 255.0f);
            colors[ImGuiCol_DockingPreview]        = ImVec4(47.0f / 255.0f, 6.0f / 255.0f, 29.0f / 255.0f, 0.7f);
            colors[ImGuiCol_DockingEmptyBg]        = ImVec4(0.20f, 0.20f, 0.20f, 0.00f);
            colors[ImGuiCol_PlotLines]             = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
            colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
            colors[ImGuiCol_PlotHistogram]         = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
            colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
            colors[ImGuiCol_TableHeaderBg]         = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
            colors[ImGuiCol_TableBorderStrong]     = ImVec4(2.0f / 255.0f, 2.0f / 255.0f, 2.0f / 255.0f, 1.0f);
            colors[ImGuiCol_TableBorderLight]      = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
            colors[ImGuiCol_TableRowBg]            = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
            colors[ImGuiCol_TableRowBgAlt]         = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
            colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
            colors[ImGuiCol_DragDropTarget]        = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
            colors[ImGuiCol_NavHighlight]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
            colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
            colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
            colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
        }
    }

    bool UIManager::LoadFont(const std::string& p_id, const std::string& p_path, float p_fontSize)
    {
        if (m_fonts.find(p_id) == m_fonts.end())
        {
            const auto& io           = ImGui::GetIO();
            ImFont*     fontInstance = io.Fonts->AddFontFromFileTTF(p_path.c_str(), p_fontSize);

            if (fontInstance)
            {
                m_fonts[p_id] = fontInstance;
                io.Fonts->Build();
                return true;
            }
        }

        return false;
    }

    bool UIManager::UnloadFont(const std::string& p_id)
    {
        if (m_fonts.find(p_id) != m_fonts.end())
        {
            m_fonts.erase(p_id);
            return true;
        }

        return false;
    }

    bool UIManager::UseFont(const std::string& p_id)
    {
        auto foundFont = m_fonts.find(p_id);

        if (foundFont != m_fonts.end())
        {
            ImGui::PopFont();
            ImGui::PushFont(m_fonts[p_id]);

            return true;
        }

        return false;
    }

    void UIManager::UseDefaultFont() { ImGui::GetIO().FontDefault = nullptr; }

    void UIManager::EnableEditorLayoutSave(bool p_value)
    {
        if (p_value)
            ImGui::GetIO().IniFilename = m_layoutSaveFilename.c_str();
        else
            ImGui::GetIO().IniFilename = nullptr;
    }

    bool UIManager::IsEditorLayoutSaveEnabled() const { return ImGui::GetIO().IniFilename != nullptr; }

    void UIManager::SetEditorLayoutSaveFilename(const std::string& p_filename)
    {
        m_layoutSaveFilename = p_filename;
        if (IsEditorLayoutSaveEnabled())
            ImGui::GetIO().IniFilename = m_layoutSaveFilename.c_str();
    }

    void UIManager::SetEditorLayoutAutosaveFrequency(float p_frequency) { ImGui::GetIO().IniSavingRate = p_frequency; }

    float UIManager::GetEditorLayoutAutosaveFrequency(float p_frequeny) { return ImGui::GetIO().IniSavingRate; }

    void UIManager::EnableDocking(bool p_value)
    {
        m_dockingState = p_value;

        if (p_value)
            ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        else
            ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_DockingEnable;
    }

    void UIManager::ResetLayout(const std::string& p_config) const { ImGui::LoadIniSettingsFromDisk(p_config.c_str()); }

    bool UIManager::IsDockingEnabled() const { return m_dockingState; }

    void UIManager::SetCanvas(Modules::Canvas& p_canvas)
    {
        RemoveCanvas();

        m_currentCanvas = &p_canvas;
    }

    void UIManager::RemoveCanvas() { m_currentCanvas = nullptr; }

    void UIManager::Render()
    {
        if (m_currentCanvas)
        {
            m_currentCanvas->Draw();
        }
    }

    void UIManager::PushCurrentFont() {}

    void UIManager::PopCurrentFont() {}
} // namespace LunarYue::UI::Core
