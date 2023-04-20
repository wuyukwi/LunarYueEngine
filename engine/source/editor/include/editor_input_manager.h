#pragma once

#include "runtime/core/math/vector2.h"

#include "runtime/function/render/render_camera.h"

#include <vector>

namespace LunarYue
{
    class LunarYueEditor;

    // エディタコマンド列挙型
    enum class EditorCommand : unsigned int
    {
        camera_left      = 1 << 0,  // A
        camera_back      = 1 << 1,  // S
        camera_foward    = 1 << 2,  // W
        camera_right     = 1 << 3,  // D
        camera_up        = 1 << 4,  // Q
        camera_down      = 1 << 5,  // E
        translation_mode = 1 << 6,  // T
        rotation_mode    = 1 << 7,  // R
        scale_mode       = 1 << 8,  // C
        exit             = 1 << 9,  // Esc
        delete_object    = 1 << 10, // Delete
    };

    // エディタ入力マネージャクラス
    class EditorInputManager
    {
    public:
        // 初期化関数
        void initialize();
        // ティック関数
        void tick(float delta_time);

    public:
        // 入力登録関数
        void registerInput();
        // カーソルの座標更新関数
        void updateCursorOnAxis(Vector2 cursor_uv);
        // エディタコマンド処理関数
        void processEditorCommand();
        // エディタモードでのキー入力関数
        void onKeyInEditorMode(int key, int scancode, int action, int mods);

        // キー入力関数
        void onKey(int key, int scancode, int action, int mods);
        // リセット関数
        void onReset();
        // カーソル位置関数
        void onCursorPos(double xpos, double ypos);
        // カーソル入力関数
        void onCursorEnter(int entered);
        // スクロール関数
        void onScroll(double xoffset, double yoffset);
        // マウスボタンクリック関数
        void onMouseButtonClicked(int key, int action);
        // ウィンドウクローズ関数
        void onWindowClosed();

        // カーソルが矩形内にあるかどうかを判定する関数
        bool isCursorInRect(Vector2 pos, Vector2 size) const;

    public:
        // エンジンウィンドウ位置取得関数
        Vector2 getEngineWindowPos() const { return m_engine_window_pos; };
        // エンジンウィンドウサイズ取得関数
        Vector2 getEngineWindowSize() const { return m_engine_window_size; };
        // カメラ速度取得関数
        float getCameraSpeed() const { return m_camera_speed; };

        // エンジンウィンドウ位置設定関数
        void setEngineWindowPos(Vector2 new_window_pos) { m_engine_window_pos = new_window_pos; };
        // エンジンウィンドウサイズ設定関数
        void setEngineWindowSize(Vector2 new_window_size) { m_engine_window_size = new_window_size; };
        // エディタコマンドリセット関数
        void resetEditorCommand() { m_editor_command = 0; }

    private:
        // エンジンウィンドウの位置
        Vector2 m_engine_window_pos {0.0f, 0.0f};
        // エンジンウィンドウのサイズ
        Vector2 m_engine_window_size {1280.0f, 768.0f};
        // マウスのX座標
        float m_mouse_x {0.0f};
        // マウスのY座標
        float m_mouse_y {0.0f};
        // カメラの速度
        float m_camera_speed {0.05f};

        // カーソルが軸上かどうかを判断するための変数
        size_t m_cursor_on_axis {3};
        // エディタコマンド
        unsigned int m_editor_command {0};
    };
} // namespace LunarYue
