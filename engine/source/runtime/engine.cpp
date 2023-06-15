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
    // Flag indicating editor mode
    bool g_is_editor_mode {false};

    // Set of component types to be updated in the editor
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

    void LunarYueEngine::initialize()
    {
        // Initialize the engine systems
        g_runtime_global_context.initSystems();
    }

    void LunarYueEngine::clear()
    {
        // Clear the engine data
    }

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
            // Measure time using std::chrono namespace
            using namespace std::chrono;

            // Get the current time
            const steady_clock::time_point tick_time_point = steady_clock::now();

            // Calculate the time difference from the last tick
            const auto time_span = duration_cast<duration<float>>(tick_time_point - m_last_tick_time_point);
            delta_time           = time_span.count(); // Convert to seconds

            // Update the last tick time to the current time
            m_last_tick_time_point = tick_time_point;
        }
        return delta_time;
    }

    bool LunarYueEngine::tickOneFrame(float delta_time)
    {
        logicalTick(delta_time);
        calculateFPS(delta_time);

        // Swap logic and rendering context data
        g_runtime_global_context.m_render_system->swapLogicRenderData();

        rendererTick(delta_time);

#ifdef ENABLE_PHYSICS_DEBUG_RENDERER
        // Render the physics world for debugging
        g_runtime_global_context.m_physics_manager->renderPhysicsWorld(delta_time);
#endif

        // Poll events
        g_runtime_global_context.m_window_system->pollEvents();

        // Set the window title
        g_runtime_global_context.m_window_system->setTitle(std::string("LunarYue - " + std::to_string(getFPS()) + " FPS").c_str());

        const bool should_window_close = g_runtime_global_context.m_window_system->shouldClose();
        return !should_window_close;
    }

    void LunarYueEngine::logicalTick(float delta_time)
    {
        // Perform logical updates
        g_runtime_global_context.m_world_manager->tick(delta_time);
        g_runtime_global_context.m_input_system->tick();
    }

    bool LunarYueEngine::rendererTick(float delta_time)
    {
        // Perform renderer updates
        g_runtime_global_context.m_render_system->tick(delta_time);
        return true;
    }

    const float LunarYueEngine::s_fps_alpha = 1.f / 100;

    void LunarYueEngine::calculateFPS(float delta_time)
    {
        // Increment frame count
        m_frame_count++;

        if (m_frame_count == 1) // First frame
        {
            m_average_duration = delta_time; // Set initial delta time as average duration
        }
        else // Subsequent frames
        {
            // Update the average duration
            m_average_duration = m_average_duration * (1 - s_fps_alpha) + delta_time * s_fps_alpha;
        }

        // Calculate FPS
        m_fps = static_cast<int>(1.f / m_average_duration);
    }
} // namespace LunarYue
