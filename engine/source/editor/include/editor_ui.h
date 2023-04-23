#pragma once

#include "runtime/function/framework/object/object.h"
#include "runtime/function/ui/window_ui.h"

#include "editor/include/editor_file_service.h"

#include <chrono>

namespace LunarYue
{
    // LunarYueエディタのUIクラス
    class EditorUI : public WindowUI
    {
    public:
        // コンストラクタ
        EditorUI();

    private:
        // ファイルコンテンツアイテムがクリックされたときの処理
        void onFileContentItemClicked(EditorFileNode* node);
        // エディタファイルアセットUIツリーの構築
        void buildEditorFileAssetsUITree(EditorFileNode* node);
        // 軸のトグルボタンを描画
        void drawAxisToggleButton(const char* string_id, bool check_state, int axis_mode);
        // クラスUIを作成
        void createClassUI(Reflection::ReflectionInstance& instance);
        // リーフノードUIを作成
        void createLeafNodeUI(Reflection::ReflectionInstance& instance);
        // リーフUIノードの親ラベルを取得
        std::string getLeafUINodeParentLabel();

        // エディタUIを表示
        void showEditorUI();
        // エディタメニューを表示
        void showEditorMenu(bool* p_open);
        // エディタワールドオブジェクトウィンドウを表示
        void showEditorWorldObjectsWindow(bool* p_open);
        // エディタファイルコンテンツウィンドウを表示
        void showEditorFileContentWindow(bool* p_open);
        // エディタゲームウィンドウを表示
        void showEditorGameWindow(bool* p_open);
        // エディタ詳細ウィンドウを表示
        void showEditorDetailWindow(bool* p_open);

        // UIのカラースタイルを設定
        void setUIColorStyle();

    public:
        // 初期化
        virtual void initialize(WindowUIInitInfo init_info) override final;
        // プリレンダー
        virtual void preRender() override final;

    private:
        // エディタUIクリエータのマップ
        std::unordered_map<std::string, std::function<void(std::string, void*)>> m_editor_ui_creator;
        // 新しいオブジェクトインデックスマップ
        std::unordered_map<std::string, unsigned int> m_new_object_index_map;
        // エディタファイルサービス
        EditorFileService m_editor_file_service;
        // 最後のファイルツリーアップデート
        std::chrono::time_point<std::chrono::steady_clock> m_last_file_tree_update;

        // 各ウィンドウのオープン状態フラグ
        bool m_editor_menu_window_open       = true;
        bool m_asset_window_open             = true;
        bool m_game_engine_window_open       = true;
        bool m_file_content_window_open      = true;
        bool m_detail_window_open            = true;
        bool m_scene_lights_window_open      = true;
        bool m_scene_lights_data_window_open = true;
    };
} // namespace LunarYue
