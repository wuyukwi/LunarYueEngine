#include "editor/include/editor_menu_bar.h"

#include "function/framework/world/world_manager.h"
#include "function/render/window_system.h"
#include "function/ui/Widgets/Visual/Separator.h"
#include "runtime/function/global/global_context.h"

using namespace LunarYue::UI::Widgets;

LunarYue::MenuBar::MenuBar()
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

void LunarYue::MenuBar::HandleShortcuts(float p_deltaTime) {}

void LunarYue::MenuBar::RegisterPanel(const std::string& p_name, UI::Panels::PanelWindow& p_panel)
{
    auto& menuItem = m_windowMenu->CreateWidget<UI::Widgets::Menu::MenuItem>(p_name, "", true, true);
    menuItem.ValueChangedEvent += [ObjectPtr = &p_panel](auto&& PH1) { ObjectPtr->SetOpened(std::forward<decltype(PH1)>(PH1)); };

    m_panels.emplace(p_name, std::make_pair(std::ref(p_panel), std::ref(menuItem)));
}

void test() {}
void LunarYue::MenuBar::CreateFileMenu()
{
    auto& fileMenu = CreateWidget<Menu::MenuList>("File");

    fileMenu.CreateWidget<Menu::MenuItem>("Save Current Level", "CTRL + N").ClickedEvent +=
        [ObjectPtr = g_runtime_global_context.m_world_manager] { ObjectPtr->saveCurrentLevel(); };

    fileMenu.CreateWidget<Menu::MenuItem>("Reload Current Level", "CTRL + S").ClickedEvent +=
        [ObjectPtr = g_runtime_global_context.m_world_manager] { ObjectPtr->reloadCurrentLevel(); };

    fileMenu.CreateWidget<Menu::MenuItem>("Exit", "ALT + F4").ClickedEvent +=
        [ObjectPtr = g_runtime_global_context.m_window_system] { ObjectPtr->setShouldClose(true); };
};

void LunarYue::MenuBar::CreateBuildMenu() {}

void LunarYue::MenuBar::CreateWindowMenu()
{
    m_windowMenu = &CreateWidget<Menu::MenuList>("Window");
    m_windowMenu->CreateWidget<Menu::MenuItem>("Close all").ClickedEvent += [this] { OpenEveryWindows(false); };
    m_windowMenu->CreateWidget<Menu::MenuItem>("Open all").ClickedEvent += [this] { OpenEveryWindows(true); };
    m_windowMenu->CreateWidget<Visual::Separator>();

    /* When the menu is opened, we update which window is marked as "Opened" or "Closed" */
    m_windowMenu->ClickedEvent += [this] { UpdateToggleableItems(); };
}

void LunarYue::MenuBar::CreateActorsMenu() {}

void LunarYue::MenuBar::CreateResourcesMenu() {}

void LunarYue::MenuBar::CreateSettingsMenu() {}

void LunarYue::MenuBar::CreateLayoutMenu() {}

void LunarYue::MenuBar::CreateHelpMenu() {}

void LunarYue::MenuBar::UpdateToggleableItems()
{
    for (auto& [name, panel] : m_panels)
        panel.second.get().checked = panel.first.get().IsOpened();
}

void LunarYue::MenuBar::OpenEveryWindows(bool p_state)
{
    for (auto& [name, panel] : m_panels)
        panel.first.get().SetOpened(p_state);
}
