#include "editor/include/editor_menu_bar.h"
#include "editor/include/editor_global_context.h"

#include "function/framework/world/world_manager.h"
#include "function/global/global_context.h"
#include "function/render/window_system.h"
#include "function/ui/Widgets/Visual/Separator.h"

using namespace LunarYue::UI::Widgets;

namespace LunarYue
{
    MenuBar::MenuBar()
    {
        CreateFileMenu();
        CreateBuildMenu();
        CreateWindowMenu();
        CreateActorsMenu();
        CreateResourcesMenu();
        CreateSettingsMenu();
        CreateLayoutMenu();
        CreateHelpMenu();
    }

    void MenuBar::HandleShortcuts(float p_deltaTime) {}

    void MenuBar::RegisterPanel(const std::string& p_name, UI::Panels::PanelWindow& p_panel)
    {
        auto& menuItem = m_window_menu->CreateWidget<Menu::MenuItem>(p_name, "", true, true);
        menuItem.ValueChangedEvent += [ObjectPtr = &p_panel](auto&& PH1) { ObjectPtr->SetOpened(std::forward<decltype(PH1)>(PH1)); };

        m_panels.emplace(p_name, std::make_pair(std::ref(p_panel), std::ref(menuItem)));
    }

    void MenuBar::CreateFileMenu()
    {
        auto& file_menu = CreateWidget<Menu::MenuList>("File");

        file_menu.CreateWidget<Menu::MenuItem>("Save Current Level").ClickedEvent +=
            [ObjectPtr = g_runtime_global_context.m_world_manager] { ObjectPtr->saveCurrentLevel(); };

        file_menu.CreateWidget<Menu::MenuItem>("Reload Current Level").ClickedEvent +=
            [ObjectPtr = g_runtime_global_context.m_world_manager] { ObjectPtr->reloadCurrentLevel(); };

        file_menu.CreateWidget<Menu::MenuItem>("Exit", "ALT + F4").ClickedEvent += [ObjectPtr = g_runtime_global_context.m_window_system] {};
    };

    void MenuBar::CreateBuildMenu() {}

    void MenuBar::CreateWindowMenu()
    {
        m_window_menu = &CreateWidget<Menu::MenuList>("Window");
        m_window_menu->CreateWidget<Menu::MenuItem>("Close all").ClickedEvent += [this] { OpenEveryWindows(false); };
        m_window_menu->CreateWidget<Menu::MenuItem>("Open all").ClickedEvent += [this] { OpenEveryWindows(true); };
        m_window_menu->CreateWidget<Visual::Separator>();

        /* When the menu is opened, we update which window is marked as "Opened" or "Closed" */
        m_window_menu->ClickedEvent += [this] { UpdateToggleableItems(); };
    }

    void MenuBar::CreateActorsMenu() {}

    void MenuBar::CreateResourcesMenu() {}

    void MenuBar::CreateSettingsMenu() {}

    void MenuBar::CreateLayoutMenu()
    {
        auto& setting_menu = CreateWidget<Menu::MenuList>("Setting");

        auto& editor_style = setting_menu.CreateWidget<Menu::MenuList>("Editor Style");
        editor_style.CreateWidget<Menu::MenuItem>("DUNE_DARK").ClickedEvent +=
            [ObjectPtr = g_editor_global_context.m_ui_manager] { ObjectPtr->ApplyStyle(UI::Core::EditorStyle::DUNE_DARK); };
        editor_style.CreateWidget<Menu::MenuItem>("ALTERNATIVE_DARK").ClickedEvent +=
            [ObjectPtr = g_editor_global_context.m_ui_manager] { ObjectPtr->ApplyStyle(UI::Core::EditorStyle::ALTERNATIVE_DARK); };
        editor_style.CreateWidget<Menu::MenuItem>("IM_CLASSIC_STYLE").ClickedEvent +=
            [ObjectPtr = g_editor_global_context.m_ui_manager] { ObjectPtr->ApplyStyle(UI::Core::EditorStyle::IM_CLASSIC_STYLE); };
        editor_style.CreateWidget<Menu::MenuItem>("IM_DARK_STYLE").ClickedEvent +=
            [ObjectPtr = g_editor_global_context.m_ui_manager] { ObjectPtr->ApplyStyle(UI::Core::EditorStyle::IM_DARK_STYLE); };
        editor_style.CreateWidget<Menu::MenuItem>("IM_LIGHT_STYLE").ClickedEvent +=
            [ObjectPtr = g_editor_global_context.m_ui_manager] { ObjectPtr->ApplyStyle(UI::Core::EditorStyle::IM_LIGHT_STYLE); };
        editor_style.CreateWidget<Menu::MenuItem>("PICCOLO_STYLE").ClickedEvent +=
            [ObjectPtr = g_editor_global_context.m_ui_manager] { ObjectPtr->ApplyStyle(UI::Core::EditorStyle::PICCOLO_STYLE); };
    }

    void MenuBar::CreateHelpMenu() {}

    void MenuBar::UpdateToggleableItems()
    {
        for (auto& [name, panel] : m_panels)
            panel.second.get().checked = panel.first.get().IsOpened();
    }

    void MenuBar::OpenEveryWindows(bool p_state)
    {
        for (auto& [name, panel] : m_panels)
            panel.first.get().SetOpened(p_state);
    }
} // namespace LunarYue
