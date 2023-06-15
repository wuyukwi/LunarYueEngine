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
        AssetBrowser(const std::string& title, bool opened, const UI::Settings::PanelWindowSettings& windowSettings);
        // fill the asset browser panels with widgets corresponding to elements in the asset folder
        void fill();

        /**
         * clear the asset browser widgets
         */
        void clear() const;

        /**
         * Refresh the asset browser widgets (clear + fill)
         */
        void refresh();

    private:
        void parseFolder(UI::Widgets::Layout::TreeNode&          root,
                         const std::filesystem::directory_entry& directory,
                         bool                                    isEngineItem,
                         bool                                    scriptFolder = false);

        void considerItem(UI::Widgets::Layout::TreeNode*          root,
                          const std::filesystem::directory_entry& entry,
                          bool                                    isEngineItem,
                          bool                                    autoOpen     = false,
                          bool                                    scriptFolder = false);

    public:
        static const std::string FILENAMES_CHARS;

    private:
        std::filesystem::path                                           m_engineAssetFolder;
        std::filesystem::path                                           m_projectAssetFolder;
        std::filesystem::path                                           m_projectScriptFolder;
        UI::Widgets::Layout::Group*                                     m_assetList;
        std::unordered_map<UI::Widgets::Layout::TreeNode*, std::string> m_pathUpdate;
    };
} // namespace LunarYue