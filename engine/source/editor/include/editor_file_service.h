#pragma once

#include <memory>
#include <string>
#include <vector>

namespace LunarYue
{
    // エディタファイルノードクラスの定義
    class EditorFileNode;
    // エディタファイルノードの配列を定義
    using EditorFileNodeArray = std::vector<std::shared_ptr<EditorFileNode>>;

    // エディタファイルノード構造体
    struct EditorFileNode
    {
        std::string                     m_file_name;   // ファイル名
        std::string                     m_file_type;   // ファイルタイプ
        std::string                     m_file_path;   // ファイルパス
        int                             m_node_depth;  // ノードの深さ
        bool                            m_folder_open; // フォルダが開いているかどうか
        EditorFileNodeArray             m_child_nodes; // 子ノードの配列
        std::shared_ptr<EditorFileNode> m_parent_node;

        // デフォルトコンストラクタ
        EditorFileNode() = default;
        // パラメータ付きコンストラクタ
        EditorFileNode(std::string name, std::string type, std::string path, int depth) :
            m_file_name(std::move(name)), m_file_type(std::move(type)), m_file_path(std::move(path)), m_node_depth(depth), m_folder_open(false)
        {}
    };

    // エディタファイルサービスクラス
    class EditorFileService
    {
        EditorFileNodeArray m_file_node_array;                            // エディタファイルノードの配列
        EditorFileNode      m_root_node {"asset", "Folder", "asset", -1}; // ルートノード
        EditorFileNode*     m_selected_folder_node = nullptr;             // 選択されたフォルダノード

    private:
        // ファイル配列をチェックする関数
        bool checkFileArray(EditorFileNode* file_node);

        // 親ノードポインタを取得する関数
        std::shared_ptr<EditorFileNode> getParentNodePtr(EditorFileNode* file_node);

    public:
        // エディタのルートノードを取得する関数
        EditorFileNode* getEditorRootNode() { return m_file_node_array.empty() ? nullptr : m_file_node_array[0].get(); }

        // 選択されたフォルダノードを設定するメソッド
        void setSelectedFolderNode(EditorFileNode* node) { m_selected_folder_node = node; }

        // 選択されたフォルダノードを取得するメソッド
        EditorFileNode* getSelectedFolderNode() const { return m_selected_folder_node; }

        // エンジンファイルツリーを構築する関数
        void buildEngineFileTree();
    };
} // namespace LunarYue
