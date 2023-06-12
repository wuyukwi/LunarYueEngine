#include "runtime/resource/asset_manager/asset_manager.h"

#include "runtime/resource/config_manager/config_manager.h"

#include "runtime/function/global/global_context.h"

#include <filesystem>

namespace LunarYue
{
    std::filesystem::path AssetManager::getFullPath(const std::string& relative_path) const
    {
        return std::filesystem::absolute(g_runtime_global_context.m_config_manager->getRootFolder() / relative_path);
    }

    std::string AssetManager::getRelativePath(const std::filesystem::path& full_path) const
    {
        const std::filesystem::path root_folder = g_runtime_global_context.m_config_manager->getRootFolder();

        const std::filesystem::path relative_path = std::filesystem::relative(full_path, root_folder);

        return relative_path.string();
    }
} // namespace LunarYue