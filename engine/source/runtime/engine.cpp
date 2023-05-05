#include "runtime/engine.h"

#include "runtime/core/base/macro.h"
#include "runtime/core/meta/reflection/reflection_register.h"

#include "runtime/function/framework/world/world_manager.h"
#include "runtime/function/global/global_context.h"
#include "runtime/function/input/input_system.h"
#include "runtime/function/particle/particle_manager.h"
#include "runtime/function/render/debugdraw/debug_draw_manager.h"
#include "runtime/function/render/render_system.h"
#include "runtime/function/render/window_system.h"

namespace LunarYue
{
    // エディタモードのフラグ
    bool g_is_editor_mode {false};
    // エディタで更新するコンポーネントタイプのセット
    std::unordered_set<std::string> g_editor_tick_component_types {};

    void LunarYueEngine::startEngine(const std::string& config_file_path)
    {
        Reflection::TypeMetaRegister::metaRegister();

        g_runtime_global_context.startSystems(config_file_path);

        LOG_INFO("engine start");
    }

    void LunarYueEngine::shutdownEngine()
    {
        LOG_INFO("engine shutdown");

        g_runtime_global_context.shutdownSystems();

        Reflection::TypeMetaRegister::metaUnregister();
    }

    void LunarYueEngine::initialize() {}
    void LunarYueEngine::clear() {}

    void LunarYueEngine::run()
    {
        std::shared_ptr<WindowSystem> window_system = g_runtime_global_context.m_window_system;
        ASSERT(window_system);

        while (!window_system->shouldClose())
        {
            const float delta_time = calculateDeltaTime();
            tickOneFrame(delta_time);
        }
    }

    float LunarYueEngine::calculateDeltaTime()
    {
        float delta_time;
        {
            // std::chrono名前空間を使用して時間を計測する
            using namespace std::chrono;

            // 現在の時間を取得
            const steady_clock::time_point tick_time_point = steady_clock::now();
            // 最後の時間との差分を計算
            const auto time_span = duration_cast<duration<float>>(tick_time_point - m_last_tick_time_point);
            delta_time           = time_span.count(); // 単位を秒に変換

            // 最後の時間を現在の時間に更新
            m_last_tick_time_point = tick_time_point;
        }
        return delta_time; // 時間の差分を返す
    }

    bool LunarYueEngine::tickOneFrame(float delta_time)
    {
        logicalTick(delta_time);
        calculateFPS(delta_time);

        // ロジックとレンダリングコンテキストのデータを交換する
        g_runtime_global_context.m_render_system->swapLogicRenderData();

        rendererTick(delta_time);

#ifdef ENABLE_PHYSICS_DEBUG_RENDERER
        // 物理ワールドのデバッグ描画
        g_runtime_global_context.m_physics_manager->renderPhysicsWorld(delta_time);
#endif

        // イベントをポーリングする
        g_runtime_global_context.m_window_system->pollEvents();

        // ウィンドウタイトルを設定する
        g_runtime_global_context.m_window_system->setTitle(std::string("LunarYue - " + std::to_string(getFPS()) + " FPS").c_str());

        const bool should_window_close = g_runtime_global_context.m_window_system->shouldClose();
        return !should_window_close;
    }

    void LunarYueEngine::logicalTick(float delta_time)
    {
        g_runtime_global_context.m_world_manager->tick(delta_time);
        g_runtime_global_context.m_input_system->tick();
    }

    bool LunarYueEngine::rendererTick(float delta_time)
    {
        g_runtime_global_context.m_render_system->tick(delta_time);
        return true;
    }

    const float LunarYueEngine::s_fps_alpha = 1.f / 100;
    void        LunarYueEngine::calculateFPS(float delta_time)
    {
        // フレームカウントをインクリメントする
        m_frame_count++;

        if (m_frame_count == 1) // 初回の場合
        {
            m_average_duration = delta_time; // 平均デュレーションに現在のデルタタイムを代入する
        }
        else // 2回目以降の場合
        {
            // 平均デュレーションを更新する
            m_average_duration = m_average_duration * (1 - s_fps_alpha) + delta_time * s_fps_alpha;
        }

        // FPSを計算する
        m_fps = static_cast<int>(1.f / m_average_duration);
    }
} // namespace LunarYue
