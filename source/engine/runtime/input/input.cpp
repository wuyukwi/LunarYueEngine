#include "input.h"
#include "../system/events.h"

namespace runtime
{
    input::input()
    {
        int      mouseX, mouseY;
        uint32_t mouseState        = SDL_GetMouseState(&mouseX, &mouseY);
        current_cursor_position_.x = mouseX;
        current_cursor_position_.y = mouseY;
        last_cursor_position_      = current_cursor_position_;

        on_platform_events.connect(this, &input::platform_events);
        on_frame_end.connect(this, &input::reset_state);
    }

    input::~input()
    {
        on_platform_events.disconnect(this, &input::platform_events);
        on_frame_end.disconnect(this, &input::reset_state);
    }

    void input::reset_state(float /*unused*/)
    {
        key_reset();

        mouse_reset();

        joystick_reset();
    }

    void input::handle_event(const SDL_Event& event)
    {
        action_mapper_.handle_event(event);

        if (key_event(event))
        {
            return;
        }

        if (mouse_event(event))
        {
            return;
        }

        if (joystick_event(event))
        {
            return;
        }
    }

    bool input::is_key_pressed(SDL_KeyCode key) { return keys_pressed_[key]; }

    bool input::is_key_pressed(SDL_KeyCode key, SDL_KeyCode modifier) { return is_key_pressed(key) && is_key_down(modifier); }

    bool input::is_key_down(SDL_KeyCode key) { return keys_down_[key]; }

    bool input::is_key_released(SDL_KeyCode key) { return keys_released_[key]; }

    bool input::is_mouse_button_pressed(uint8_t button) { return mouse_buttons_pressed_[button]; }

    bool input::is_mouse_button_down(uint8_t button) { return mouse_buttons_down_[button]; }

    bool input::is_mouse_button_released(uint8_t button) { return mouse_buttons_released_[button]; }

    bool input::is_joystick_connected(int32_t joystick_id) { return joysticks_connected_[joystick_id]; }

    bool input::is_joystick_active(int32_t joystick_id) { return joysticks_active_[joystick_id]; }

    bool input::is_joystick_disconnected(int32_t joystick_id) { return joysticks_disconnected_[joystick_id]; }

    bool input::is_joystick_button_pressed(int32_t joystick_id, uint8_t button)
    {
        return joystick_buttons_pressed_[std::pair<int32_t, uint8_t>(joystick_id, button)];
    }

    bool input::is_joystick_button_down(int32_t joystick_id, uint8_t button) { return joystick_buttons_down_[{joystick_id, button}]; }

    bool input::is_joystick_button_released(int32_t joystick_id, uint8_t button) { return joystick_buttons_released_[{joystick_id, button}]; }

    float input::get_joystick_axis_position(int32_t joystick_id, SDL_GameControllerAxis axis)
    {
        return joystick_axis_positions_[{joystick_id, axis}];
    }

    void input::key_reset()
    {
        for (auto& it : keys_pressed_)
        {
            if (it.second)
            {
                keys_down_[it.first] = true;
                it.second            = false;
            }
        }

        for (auto& it : keys_released_)
        {
            if (it.second)
            {
                it.second = false;
            }
        }
    }

    bool input::key_event(const SDL_Event& event)
    {
        if (event.type == SDL_KEYDOWN)
        {
            SDL_Keycode keycode     = event.key.keysym.sym;
            keys_pressed_[keycode]  = !keys_down_[keycode];
            keys_released_[keycode] = false;
            return true;
        }
        if (event.type == SDL_KEYUP)
        {
            SDL_Keycode keycode     = event.key.keysym.sym;
            keys_pressed_[keycode]  = false;
            keys_down_[keycode]     = false;
            keys_released_[keycode] = true;
            return true;
        }
        return false;
    }

    void input::mouse_reset()
    {
        mouse_move_event_     = false;
        last_cursor_position_ = current_cursor_position_;
        for (auto& it : mouse_buttons_pressed_)
        {
            if (it.second)
            {
                mouse_buttons_down_[it.first] = true;
                it.second                     = false;
            }
        }

        for (auto& it : mouse_buttons_released_)
        {
            if (it.second)
            {
                it.second = false;
            }
        }

        if (mouse_wheel_scrolled_)
        {
            mouse_wheel_scrolled_ = false;
            mouse_scroll_delta_   = 0.0f;
        }
    }

    bool input::mouse_event(const SDL_Event& event)
    {
        if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            mouse_buttons_pressed_[event.button.button]  = !mouse_buttons_down_[event.button.button];
            mouse_buttons_released_[event.button.button] = false;
            return true;
        }
        if (event.type == SDL_MOUSEBUTTONUP)
        {
            mouse_buttons_pressed_[event.button.button]  = false;
            mouse_buttons_down_[event.button.button]     = false;
            mouse_buttons_released_[event.button.button] = true;
            return true;
        }
        if (event.type == SDL_MOUSEMOTION)
        {
            last_cursor_position_ = current_cursor_position_;
            ipoint32_t mouse;
            mouse.x                  = event.motion.x;
            mouse.y                  = event.motion.y;
            current_cursor_position_ = mouse;
            mouse_move_event_        = true;
            return true;
        }
        if (event.type == SDL_MOUSEWHEEL)
        {
            mouse_wheel_scrolled_ = true;
            mouse_scroll_delta_   = event.wheel.preciseY;
            return true;
        }
        return false;
    }

    void input::joystick_reset()
    {
        for (auto& it : joystick_buttons_pressed_)
        {
            if (it.second)
            {
                joystick_buttons_down_[it.first] = true;
                it.second                        = false;
            }
        }

        for (auto& it : joystick_buttons_released_)
        {
            if (it.second)
            {
                it.second = false;
            }
        }

        for (auto& it : joysticks_connected_)
        {
            if (it.second)
            {
                joysticks_active_[it.first] = true;
                it.second                   = false;
            }
        }

        for (auto& it : joysticks_disconnected_)
        {
            if (it.second)
            {
                it.second = false;
            }
        }
    }

    bool input::joystick_event(const SDL_Event& event)
    {
        if (event.type == SDL_JOYDEVICEADDED)
        {
            joysticks_connected_[event.jdevice.which]    = !joysticks_active_[event.jdevice.which];
            joysticks_disconnected_[event.jdevice.which] = false;
            return true;
        }
        if (event.type == SDL_JOYDEVICEREMOVED)
        {
            joysticks_connected_[event.jdevice.which]    = false;
            joysticks_active_[event.jdevice.which]       = false;
            joysticks_disconnected_[event.jdevice.which] = true;
            return true;
        }
        if (event.type == SDL_JOYBUTTONDOWN)
        {
            std::pair<int32_t, uint8_t> k(event.jbutton.which, event.jbutton.button);
            joystick_buttons_pressed_[k]  = !joystick_buttons_down_[k];
            joystick_buttons_released_[k] = false;
            return true;
        }
        if (event.type == SDL_JOYBUTTONUP)
        {
            std::pair<int32_t, uint8_t> k(event.jbutton.which, event.jbutton.button);
            joystick_buttons_pressed_[k]  = false;
            joystick_buttons_down_[k]     = false;
            joystick_buttons_released_[k] = true;
            return true;
        }
        if (event.type == SDL_JOYAXISMOTION)
        {
            std::pair<int32_t, uint8_t> k(event.jaxis.which, event.jaxis.axis);

            joystick_axis_positions_[k] = static_cast<float>(event.jaxis.value) / 32767.0f;
            return true;
        }
        return false;
    }

    ipoint32_t input::get_cursor_delta_move() const
    {
        return ipoint32_t {get_current_cursor_position().x - get_last_cursor_position().x,
                           get_current_cursor_position().y - get_last_cursor_position().y};
    }

    void input::platform_events(const std::vector<SDL_Event>& events)
    {
        for (const auto& e : events)
        {
            handle_event(e);
        }
    }
} // namespace runtime
