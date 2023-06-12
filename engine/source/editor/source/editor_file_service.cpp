#include "editor/include/editor_file_service.h"

#include "runtime/platform/file_service/file_service.h"
#include "runtime/platform/path/path.h"

#include "runtime/resource/asset_manager/asset_manager.h"
#include "runtime/resource/config_manager/config_manager.h"

#include "runtime/function/global/global_context.h"

namespace LunarYue
{
    void EditorFileService::buildEngineFileTree()
    {
        //// アセットフォルダの取得
        // std::string asset_folder = g_runtime_global_context.m_config_manager->getAssetFolder().generic_string();
        //// アセットフォルダ内のファイルパスを取得
        // const std::vector<std::filesystem::path> file_paths = g_runtime_global_context.m_file_system->getFiles(asset_folder);
        // std::vector<std::vector<std::string>>    all_file_segments;
        // for (const auto& path : file_paths)
        //{
        //     // 相対パスを取得
        //     const std::filesystem::path& relative_path = Path::getRelativePath(asset_folder, path);
        //     // パスセグメントを取得
        //     all_file_segments.emplace_back(Path::getPathSegments(relative_path));
        // }

        // std::vector<std::shared_ptr<EditorFileNode>> node_array;

        //// ファイルノード配列をクリア
        // m_file_node_array.clear();
        // auto root_node = std::make_shared<EditorFileNode>();
        //*root_node     = m_root_node;
        // m_file_node_array.push_back(root_node);

        // const int all_file_segments_count = all_file_segments.size();
        // for (int file_index = 0; file_index < all_file_segments_count; file_index++)
        //{
        //     int depth = 0;
        //     node_array.clear();
        //     node_array.push_back(root_node);
        //     const int file_segment_count = all_file_segments[file_index].size();

        //    for (int file_segment_index = 0; file_segment_index < file_segment_count; file_segment_index++)
        //    {
        //        auto file_node         = std::make_shared<EditorFileNode>();
        //        file_node->m_file_name = all_file_segments[file_index][file_segment_index];

        //        if (depth < file_segment_count - 1)
        //        {
        //            file_node->m_file_type = "Folder";
        //        }
        //        else
        //        {
        //            const auto& extensions = Path::getFileExtensions(file_paths[file_index]);
        //            file_node->m_file_type = std::get<0>(extensions);
        //            if (file_node->m_file_type.empty())
        //                continue;

        //            if (file_node->m_file_type == ".json")
        //            {
        //                file_node->m_file_type = std::get<1>(extensions);
        //                if (file_node->m_file_type == ".component")
        //                {
        //                    file_node->m_file_type = std::get<2>(extensions) + std::get<1>(extensions);
        //                }
        //            }
        //            file_node->m_file_type = file_node->m_file_type.substr(1);
        //            file_node->m_file_path = file_paths[file_index].generic_string();
        //        }
        //        file_node->m_node_depth = depth;
        //        node_array.push_back(file_node);

        //        bool       node_exists = checkFileArray(file_node.get());
        //        static int check       = 0;
        //        LOG_DEBUG(std::to_string(check));
        //        check++;

        //        if (node_exists == false)
        //        {
        //            m_file_node_array.push_back(file_node);
        //        }

        //        auto parent_node_ptr = getParentNodePtr(node_array[depth].get());
        //        if (parent_node_ptr != nullptr && node_exists == false)
        //        {
        //            parent_node_ptr->m_child_nodes.push_back(file_node);

        //            if (!file_node->m_parent_node)
        //            {
        //                file_node->m_parent_node = parent_node_ptr;
        //            }
        //        }
        //        depth++;
        //    }
        //}
    }

    bool EditorFileService::checkFileArray(EditorFileNode* file_node)
    {
        for (const auto& editor_node : m_file_node_array)
        {
            if (editor_node->m_file_name == file_node->m_file_name && editor_node->m_node_depth == file_node->m_node_depth)
            {
                return true;
            }
        }
        return false;
    }

    std::shared_ptr<EditorFileNode> EditorFileService::getParentNodePtr(EditorFileNode* file_node)
    {
        for (const auto& editor_file_node : m_file_node_array)
        {
            if (editor_file_node->m_file_name == file_node->m_file_name && editor_file_node->m_node_depth == file_node->m_node_depth)
            {
                return editor_file_node;
            }
        }
        return nullptr;
    }
} // namespace LunarYue
