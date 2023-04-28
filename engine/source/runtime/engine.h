#pragma once

#include <chrono>
#include <string>
#include <unordered_set>

namespace LunarYue
{
    extern bool                            g_is_editor_mode;
    extern std::unordered_set<std::string> g_editor_tick_component_types;

    class LunarYueEngine
    {
        friend class LunarYueEditor;

        // FPS 計算のための定数
        static const float s_fps_alpha;

    public:
        // エンジンの開始
        static void startEngine(const std::string& config_file_path);
        // エンジンのシャットダウン
        void shutdownEngine();

        // 初期化
        void initialize();
        // クリア
        void clear();

        // 終了フラグの取得
        bool isQuit() const { return m_is_quit; }
        // 実行
        void run();
        // 1フレームごとの更新
        bool tickOneFrame(float delta_time);

        // 現在の FPS の取得
        int getFPS() const { return m_fps; }

    protected:
        // 論理更新
        void logicalTick(float delta_time);
        // レンダリング更新
        bool rendererTick(float delta_time);

        // FPS 計算
        void calculateFPS(float delta_time);

        /**
         *  各フレームで1回だけ呼び出される
         */
        // デルタ時間の計算
        float calculateDeltaTime();

    protected:
        // 終了フラグ
        bool m_is_quit {false};

        // 前回の更新時点
        std::chrono::steady_clock::time_point m_last_tick_time_point {std::chrono::steady_clock::now()};

        // 平均フレーム時間
        float m_average_duration {0.f};
        // フレームカウント
        int m_frame_count {0};
        // FPS
        int m_fps {0};
    };

} // namespace LunarYue
