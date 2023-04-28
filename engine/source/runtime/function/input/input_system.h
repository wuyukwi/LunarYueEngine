#pragma once

#include "runtime/core/math/math.h"

namespace LunarYue
{
    enum class GameCommand : unsigned int
    {
        forward     = 1 << 0,                 // W
        backward    = 1 << 1,                 // S
        left        = 1 << 2,                 // A
        right       = 1 << 3,                 // D
        jump        = 1 << 4,                 // SPACE
        squat       = 1 << 5,                 // not implemented yet
        sprint      = 1 << 6,                 // LEFT SHIFT
        fire        = 1 << 7,                 // not implemented yet
        free_carema = 1 << 8,                 // F
        invalid     = (unsigned int)(1 << 31) // lost focus
    };

    extern unsigned int k_complement_control_command;

    class InputSystem
    {
    public:
        // キーイベントを処理
        void onKey(int key, int scancode, int action, int mods);
        // カーソル位置イベントを処理
        void onCursorPos(double current_cursor_x, double current_cursor_y);

        // システムの初期化
        void initialize();
        // システムの更新
        void tick();
        // システムのクリア
        void clear();

        // カーソルのデルタX
        int m_cursor_delta_x {0};
        // カーソルのデルタY
        int m_cursor_delta_y {0};

        // カーソルのデルタ角度（ヨー）
        Radian m_cursor_delta_yaw {0};
        // カーソルのデルタ角度（ピッチ）
        Radian m_cursor_delta_pitch {0};

        // ゲームコマンドをリセット
        void resetGameCommand() { m_game_command = 0; }
        // ゲームコマンドを取得
        unsigned int getGameCommand() const { return m_game_command; }

    private:
        // ゲームモードでのキーイベントを処理
        void onKeyInGameMode(int key, int scancode, int action, int mods);

        // カーソルのデルタ角度を計算
        void calculateCursorDeltaAngles();

        // ゲームコマンド
        unsigned int m_game_command {0};

        // 最後のカーソルX座標
        int m_last_cursor_x {0};
        // 最後のカーソルY座標
        int m_last_cursor_y {0};
    };

} // namespace LunarYue
