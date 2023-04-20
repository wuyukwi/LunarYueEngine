#pragma once

namespace LunarYue
{
    // グローバルコンテキスト初期化情報構造体
    struct EditorGlobalContextInitInfo
    {
        class WindowSystem*   window_system;  // ウィンドウシステム
        class RenderSystem*   render_system;  // レンダーシステム
        class LunarYueEngine* engine_runtime; // エンジンランタイム
    };

    // エディタグローバルコンテキストクラス
    class EditorGlobalContext
    {
    public:
        class EditorSceneManager* m_scene_manager {nullptr};  // シーンマネージャ
        class EditorInputManager* m_input_manager {nullptr};  // 入力マネージャ
        class RenderSystem*       m_render_system {nullptr};  // レンダーシステム
        class WindowSystem*       m_window_system {nullptr};  // ウィンドウシステム
        class LunarYueEngine*     m_engine_runtime {nullptr}; // エンジンランタイム

    public:
        // 初期化関数
        void initialize(const EditorGlobalContextInitInfo& init_info);
        // クリア関数
        void clear();
    };

    // エディタグローバルコンテキスト外部変数
    extern EditorGlobalContext g_editor_global_context;
} // namespace LunarYue
