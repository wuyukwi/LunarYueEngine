#include "editor/include/editor_ui.h"
#include "editor/include/editor_asset_browser.h"
#include "editor/include/editor_detail_window.h"
#include "editor/include/editor_global_context.h"
#include "editor/include/editor_object_window.h"
#include "editor/include/editor_scene_view.h"

#include "runtime/core/base/macro.h"
#include "runtime/function/ui/Settings/PanelWindowSettings.h"
#include "runtime/function/ui/ui_manager.h"
#include "runtime/resource/asset_manager/asset_manager.h"

namespace LunarYue
{

    // コンストラクタ
    EditorUI::EditorUI() {}

    void EditorUI::setupUI()
    {
        UI::Settings::PanelWindowSettings settings;
        settings.closable    = true;
        settings.collapsable = true;
        settings.dockable    = true;

        g_editor_global_context.m_panels_manager->CreatePanel<MenuBar>("Menu Bar");
        g_editor_global_context.m_panels_manager->CreatePanel<SceneView>("Scene View", true, settings);
        g_editor_global_context.m_panels_manager->CreatePanel<AssetBrowser>("Asset Browser", true, settings);
        g_editor_global_context.m_panels_manager->CreatePanel<ObjectWindow>("Object Window", true, settings);
        g_editor_global_context.m_panels_manager->CreatePanel<DetailWindow>("Detail Window", true, settings);

        m_canvas.MakeDockspace(true);
        g_editor_global_context.m_ui_manager->SetCanvas(m_canvas);
    }

    void EditorUI::initialize()
    {
        g_editor_global_context.m_panels_manager = std::make_shared<PanelsManager>(m_canvas);

        setupUI();
    }

    void EditorUI::preRender()
    {
        g_editor_global_context.m_ui_manager->Render();

        auto& sceneView = g_editor_global_context.m_panels_manager->GetPanelAs<SceneView>("Scene View");

        if (sceneView.IsOpened())
            sceneView._Render_Impl();
    }
} // namespace LunarYue
