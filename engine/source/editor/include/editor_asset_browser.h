#pragma once

#include <filesystem>
#include <unordered_map>

#include "function/ui/Panels/PanelWindow.h"
#include "function/ui/Widgets/Layout/Group.h"
#include "function/ui/Widgets/Layout/TreeNode.h"

namespace LunarYue
{
    class AssetBrowser : public UI::Panels::PanelWindow
    {
    public:
        AssetBrowser(const std::string& p_title, bool p_opened, const UI::Settings::PanelWindowSettings& p_windowSettings);
        /**
         * Fill the asset browser panels with widgets corresponding to elements in the asset folder
         */
        void Fill();

        /**
         * Clear the asset browser widgets
         */
        void Clear();

        /**
         * Refresh the asset browser widgets (Clear + Fill)
         */
        void Refresh();

    private:
        void ParseFolder(UI::Widgets::Layout::TreeNode&          p_root,
                         const std::filesystem::directory_entry& p_directory,
                         bool                                    p_isEngineItem,
                         bool                                    p_scriptFolder = false);
        void ConsiderItem(UI::Widgets::Layout::TreeNode*          p_root,
                          const std::filesystem::directory_entry& p_entry,
                          bool                                    p_isEngineItem,
                          bool                                    p_autoOpen     = false,
                          bool                                    p_scriptFolder = false);

    public:
        static const std::string __FILENAMES_CHARS;

    private:
        std::filesystem::path                                           m_engineAssetFolder;
        std::filesystem::path                                           m_projectAssetFolder;
        std::filesystem::path                                           m_projectScriptFolder;
        UI::Widgets::Layout::Group*                                     m_assetList;
        std::unordered_map<UI::Widgets::Layout::TreeNode*, std::string> m_pathUpdate;
    };
} // namespace LunarYue