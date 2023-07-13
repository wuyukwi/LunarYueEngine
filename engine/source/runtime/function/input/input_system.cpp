#include "runtime/function/input/input_system.h"

#include "core/base/macro.h"

#include "runtime/engine.h"
#include "runtime/function/global/global_context.h"
#include "runtime/function/render/render_camera.h"
#include "runtime/function/render/render_system.h"
#include "runtime/function/render/window_system.h"

#include <GLFW/glfw3.h>

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

        if (action == GLFW_PRESS)
        {
            switch (key)
            {
                case GLFW_KEY_ESCAPE:
                    // close();
                    break;
                case GLFW_KEY_R:
                    break;
                case GLFW_KEY_A:
                    m_game_command |= (unsigned int)GameCommand::left;
                    break;
                case GLFW_KEY_S:
                    m_game_command |= (unsigned int)GameCommand::backward;
                    break;
                case GLFW_KEY_W:
                    m_game_command |= (unsigned int)GameCommand::forward;
                    break;
                case GLFW_KEY_D:
                    m_game_command |= (unsigned int)GameCommand::right;
                    break;
                case GLFW_KEY_SPACE:
                    m_game_command |= (unsigned int)GameCommand::jump;
                    break;
                case GLFW_KEY_LEFT_CONTROL:
                    m_game_command |= (unsigned int)GameCommand::squat;
                    break;
                case GLFW_KEY_LEFT_ALT: {
                }
                break;
                case GLFW_KEY_LEFT_SHIFT:
                    m_game_command |= (unsigned int)GameCommand::sprint;
                    break;
                case GLFW_KEY_F:
                    m_game_command ^= (unsigned int)GameCommand::free_carema;
                    break;
                default:
                    break;
            }
        }
        else if (action == GLFW_RELEASE)
        {
            switch (key)
            {
                case GLFW_KEY_ESCAPE:
                    // close();
                    break;
                case GLFW_KEY_R:
                    break;
                case GLFW_KEY_W:
                    m_game_command &= (k_complement_control_command ^ (unsigned int)GameCommand::forward);
                    break;
                case GLFW_KEY_S:
                    m_game_command &= (k_complement_control_command ^ (unsigned int)GameCommand::backward);
                    break;
                case GLFW_KEY_A:
                    m_game_command &= (k_complement_control_command ^ (unsigned int)GameCommand::left);
                    break;
                case GLFW_KEY_D:
                    m_game_command &= (k_complement_control_command ^ (unsigned int)GameCommand::right);
                    break;
                case GLFW_KEY_LEFT_CONTROL:
                    m_game_command &= (k_complement_control_command ^ (unsigned int)GameCommand::squat);
                    break;
                case GLFW_KEY_LEFT_SHIFT:
                    m_game_command &= (k_complement_control_command ^ (unsigned int)GameCommand::sprint);
                    break;
                default:
                    break;
            }
        }
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
