#pragma once
#include "input_mapping.hpp"

#include <core/common_lib/basetypes.hpp>
#include <core/signals/event.hpp>

#include <map>
#include <unordered_map>

namespace runtime
{
    using action_map_t = std::unordered_map<std::string, std::map<action_type, hpp::event<void(const SDL_Event&)>>>;

    struct action_mapper
    {
        ///
        keyboard_mapper keyboard_map;
        ///
        mouse_button_mapper mouse_button_map;
        ///
        mouse_wheel_mapper mouse_wheel_map;
        ///
        touch_finger_mapper touch_finger_map;
        ///
        joystick_button_mapper joystick_button_map;
        ///
        event_mapper event_map;
        ///
        action_map_t actions;

        void handle_event(const SDL_Event& event)
        {
            auto trigger_callbacks = [this](auto& mapper, const SDL_Event& event) {
                auto mappings = mapper.get_mapping(event);
                for (auto& action : mappings.actions)
                {
                    auto& mapped_event = actions[action][mappings.type];
                    mapped_event(event);
                }
            };

            trigger_callbacks(keyboard_map, event);
            trigger_callbacks(mouse_button_map, event);
            trigger_callbacks(mouse_wheel_map, event);
            trigger_callbacks(touch_finger_map, event);
            trigger_callbacks(joystick_button_map, event);
            trigger_callbacks(event_map, event);
        }
    };

    // 	auto& input = core::get_subsystem<runtime::input>();
    // 	auto& mappings = input.get_mappings();
    //
    //	// You can map different type of events to the same action
    // 	mappings.event_map.map("some_action",
    //  mml::platform_event::text_entered);
    // 	mappings.mouse_button_map.map("some_action", mml::mouse::Right);
    // 	mappings.keyboard_map.map("some_action", mml::keyboard::Space);
    //
    // 	//you can subscribe to a callback for a specific event and action type
    // 	mappings.actions["some_action"][action_type::pressed].connect([](const
    //  mml::platform_event& e)
    // 	{
    // 		//do some stuff
    // 	});
    // 	mappings.actions["some_action"][action_type::changed].connect([](const
    //  mml::platform_event& e)
    // 	{
    // 		//do some stuff
    // 	});
    class input
    {
    public:
        input();
        ~input();

        void platform_events(const std::pair<std::uint32_t, bool>& info, const std::vector<SDL_Event>& events);

        //-----------------------------------------------------------------------------
        //  Name : get_mappings ()
        /// <summary>
        ///
        ///
        ///
        /// </summary>
        //-----------------------------------------------------------------------------
        inline action_mapper& get_mappings() { return action_mapper_; }

        //-----------------------------------------------------------------------------
        //  Name : reset_state ()
        /// <summary>
        ///
        ///
        ///
        /// </summary>
        //-----------------------------------------------------------------------------
        void reset_state(delta_t);

        //-----------------------------------------------------------------------------
        //  Name : handle_event ()
        /// <summary>
        ///
        ///
        ///
        /// </summary>
        //-----------------------------------------------------------------------------
        void handle_event(const SDL_Event& event);

        //-----------------------------------------------------------------------------
        //  Name : is_key_pressed ()
        /// <summary>
        ///
        ///
        ///
        /// </summary>
        //-----------------------------------------------------------------------------
        bool is_key_pressed(SDL_KeyCode key);
        bool is_key_pressed(SDL_KeyCode key, SDL_KeyCode modifier);
        //-----------------------------------------------------------------------------
        //  Name : is_key_down ()
        /// <summary>
        ///
        ///
        ///
        /// </summary>
        //-----------------------------------------------------------------------------
        bool is_key_down(SDL_KeyCode key);

        //-----------------------------------------------------------------------------
        //  Name : is_key_released ()
        /// <summary>
        ///
        ///
        ///
        /// </summary>
        //-----------------------------------------------------------------------------
        bool is_key_released(SDL_KeyCode key);

        //-----------------------------------------------------------------------------
        //  Name : mouseMoved ()
        /// <summary>
        ///
        ///
        ///
        /// </summary>
        //-----------------------------------------------------------------------------
        inline bool mouse_moved() const { return mouse_move_event_; }

        //-----------------------------------------------------------------------------
        //  Name : get_cursor_position ()
        /// <summary>
        ///
        ///
        ///
        /// </summary>
        //-----------------------------------------------------------------------------
        inline const ipoint32_t& get_current_cursor_position() const { return current_cursor_position_; }

        //-----------------------------------------------------------------------------
        //  Name : get_cursor_last_position ()
        /// <summary>
        ///
        ///
        ///
        /// </summary>
        //-----------------------------------------------------------------------------
        inline const ipoint32_t& get_last_cursor_position() const { return last_cursor_position_; }

        //-----------------------------------------------------------------------------
        //  Name : get_cursor_delta_move ()
        /// <summary>
        ///
        ///
        ///
        /// </summary>
        //-----------------------------------------------------------------------------
        ipoint32_t get_cursor_delta_move() const;

        //-----------------------------------------------------------------------------
        //  Name : is_mouse_button_pressed ()
        /// <summary>
        ///
        ///
        ///
        /// </summary>
        //-----------------------------------------------------------------------------
        bool is_mouse_button_pressed(uint8_t button);

        //-----------------------------------------------------------------------------
        //  Name : is_mouse_button_down ()
        /// <summary>
        ///
        ///
        ///
        /// </summary>
        //-----------------------------------------------------------------------------
        bool is_mouse_button_down(uint8_t button);

        //-----------------------------------------------------------------------------
        //  Name : is_mouse_button_released ()
        /// <summary>
        ///
        ///
        ///
        /// </summary>
        //-----------------------------------------------------------------------------
        bool is_mouse_button_released(uint8_t button);

        //-----------------------------------------------------------------------------
        //  Name : is_mouse_wheel_scrolled ()
        /// <summary>
        ///
        ///
        ///
        /// </summary>
        //-----------------------------------------------------------------------------
        inline bool is_mouse_wheel_scrolled() const { return mouse_wheel_scrolled_; }

        //-----------------------------------------------------------------------------
        //  Name : get_mouse_wheel_scroll ()
        /// <summary>
        ///
        ///
        ///
        /// </summary>
        //-----------------------------------------------------------------------------
        inline float get_mouse_wheel_scroll_delta_move() const { return mouse_scroll_delta_; }

        //-----------------------------------------------------------------------------
        //  Name : is_joystick_connected ()
        /// <summary>
        ///
        ///
        ///
        /// </summary>
        //-----------------------------------------------------------------------------
        bool is_joystick_connected(int32_t joystick_id);

        //-----------------------------------------------------------------------------
        //  Name : is_joystick_active ()
        /// <summary>
        ///
        ///
        ///
        /// </summary>
        //-----------------------------------------------------------------------------
        bool is_joystick_active(int32_t joystick_id);

        //-----------------------------------------------------------------------------
        //  Name : is_joystick_disconnected ()
        /// <summary>
        ///
        ///
        ///
        /// </summary>
        //-----------------------------------------------------------------------------
        bool is_joystick_disconnected(int32_t joystick_id);

        //-----------------------------------------------------------------------------
        //  Name : is_joystick_button_pressed ()
        /// <summary>
        ///
        ///
        ///
        /// </summary>
        //-----------------------------------------------------------------------------
        bool is_joystick_button_pressed(int32_t joystick_id, uint8_t button);

        //-----------------------------------------------------------------------------
        //  Name : is_joystick_button_down ()
        /// <summary>
        ///
        ///
        ///
        /// </summary>
        //-----------------------------------------------------------------------------
        bool is_joystick_button_down(int32_t joystick_id, uint8_t button);

        //-----------------------------------------------------------------------------
        //  Name : is_joystick_button_released ()
        /// <summary>
        ///
        ///
        ///
        /// </summary>
        //-----------------------------------------------------------------------------
        bool is_joystick_button_released(int32_t joystick_id, uint8_t button);

        //-----------------------------------------------------------------------------
        //  Name : get_joystick_axis_position ()
        /// <summary>
        ///
        ///
        ///
        /// </summary>
        //-----------------------------------------------------------------------------
        float get_joystick_axis_position(int32_t joystick_id, SDL_GameControllerAxis axis);

        //-----------------------------------------------------------------------------
        //  Name : mouse_event ()
        /// <summary>
        ///
        ///
        ///
        /// </summary>
        //-----------------------------------------------------------------------------
        bool mouse_event(const SDL_Event& event);

    private:
        //-----------------------------------------------------------------------------
        //  Name : key_reset ()
        /// <summary>
        ///
        ///
        ///
        /// </summary>
        //-----------------------------------------------------------------------------
        void key_reset();

        //-----------------------------------------------------------------------------
        //  Name : key_event ()
        /// <summary>
        ///
        ///
        ///
        /// </summary>
        //-----------------------------------------------------------------------------
        bool key_event(const SDL_Event& event);

        //-----------------------------------------------------------------------------
        //  Name : mouse_reset ()
        /// <summary>
        ///
        ///
        ///
        /// </summary>
        //-----------------------------------------------------------------------------
        void mouse_reset();

        //-----------------------------------------------------------------------------
        //  Name : joystick_reset ()
        /// <summary>
        ///
        ///
        ///
        /// </summary>
        //-----------------------------------------------------------------------------
        void joystick_reset();

        //-----------------------------------------------------------------------------
        //  Name : joystick_event ()
        /// <summary>
        ///
        ///
        ///
        /// </summary>
        //-----------------------------------------------------------------------------
        bool joystick_event(const SDL_Event& event);

        ///
        action_mapper action_mapper_;
        ///
        bool mouse_move_event_ = false;
        ///
        bool mouse_wheel_scrolled_ = false;
        ///
        float mouse_scroll_delta_ = 0.0f;
        ///
        ipoint32_t current_cursor_position_;
        ///
        ipoint32_t last_cursor_position_;
        ///
        std::map<uint8_t, bool> mouse_buttons_pressed_;
        ///
        std::map<uint8_t, bool> mouse_buttons_down_;
        ///
        std::map<uint8_t, bool> mouse_buttons_released_;
        ///
        std::map<SDL_Keycode, bool> keys_pressed_;
        ///
        std::map<SDL_Keycode, bool> keys_down_;
        ///
        std::map<SDL_Keycode, bool> keys_released_;
        ///
        std::map<int32_t, bool> joysticks_connected_;
        ///
        std::map<int32_t, bool> joysticks_active_;
        ///
        std::map<int32_t, bool> joysticks_disconnected_;
        ///
        std::map<std::pair<int32_t, uint8_t>, bool> joystick_buttons_pressed_;
        ///
        std::map<std::pair<int32_t, uint8_t>, bool> joystick_buttons_down_;
        ///
        std::map<std::pair<int32_t, uint8_t>, bool> joystick_buttons_released_;
        ///
        std::map<std::pair<int32_t, uint8_t>, float> joystick_axis_positions_;
    };
} // namespace runtime
