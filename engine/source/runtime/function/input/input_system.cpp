#include "runtime/function/input/input_system.h"

#include "core/base/macro.h"

#include "runtime/engine.h"
#include "runtime/function/global/global_context.h"
#include "runtime/function/render/render_camera.h"
#include "runtime/function/render/render_system.h"
#include "runtime/function/render/window_system.h"

namespace LunarYue
{
    // ï‚äÆêßå‰ÉRÉ}ÉìÉh
    unsigned int k_complement_control_command = 0xFFFFFFFF;

    void InputSystem::onKey(int key, int scancode, int action, int mods)
    {
        if (!g_is_editor_mode)
        {
            onKeyInGameMode(key, scancode, action, mods);
        }
    }

    void InputSystem::onKeyInGameMode(int key, int scancode, int action, int mods)
    {
        m_game_command &= (k_complement_control_command ^ (unsigned int)GameCommand::jump);
    }

    void InputSystem::onCursorPos(double current_cursor_x, double current_cursor_y) {}

    void InputSystem::initialie() {}

    void InputSystem::clear()
    {
        m_cursor_delta_x = 0;
        m_cursor_delta_y = 0;
    }

    void InputSystem::calculateCursorDeltaAngles() {}

    void InputSystem::tick() {}
} // namespace LunarYue
