#pragma once

#include <core/common_lib/hash.hpp>

#include "SDL_events.h"

#include <map>
#include <string>
#include <vector>

namespace std
{
    template<typename S, typename T>
    struct hash<pair<S, T>>
    {
        inline size_t operator()(const pair<S, T>& v) const
        {
            size_t seed = 0;
            utils::hash_combine(seed, v.first);
            utils::hash_combine(seed, v.second);
            return seed;
        }
    };
} // namespace std

enum class action_type : unsigned int
{
    not_mapped,
    pressed,
    changed,
    released
};

struct input_mapping
{
    action_type              type = action_type::not_mapped;
    std::vector<std::string> actions;
};

template<typename T>
class input_mapper
{
public:
    //-----------------------------------------------------------------------------
    //  Name : map ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    void map(const std::string& action, T input) { bindings_[input].push_back(action); }

protected:
    /// mappings
    std::map<T, std::vector<std::string>> bindings_;
};

struct keyboard_mapper : public input_mapper<int32_t>
{
    input_mapping get_mapping(const SDL_Event& e)
    {
        input_mapping binds;
        if (e.type == SDL_KEYDOWN)
        {
            binds.actions = bindings_[e.key.keysym.sym];
            binds.type    = action_type::pressed;
        }
        if (e.type == SDL_KEYUP)
        {
            binds.actions = bindings_[e.key.keysym.sym];
            binds.type    = action_type::released;
        }
        return binds;
    }
};

struct mouse_button_mapper : public input_mapper<uint8_t>
{
    input_mapping get_mapping(const SDL_Event& e)
    {
        input_mapping binds;
        if (e.type == SDL_MOUSEBUTTONDOWN)
        {
            binds.actions = bindings_[e.button.button];
            binds.type    = action_type::pressed;
        }
        if (e.type == SDL_MOUSEBUTTONUP)
        {
            binds.actions = bindings_[e.button.button];
            binds.type    = action_type::released;
        }
        return binds;
    }
};

struct mouse_wheel_mapper : public input_mapper<std::pair<float, float>>
{
    input_mapping get_mapping(const SDL_Event& e)
    {
        input_mapping binds;
        if (e.type == SDL_MOUSEWHEEL)
        {
            binds.actions = bindings_[{e.wheel.preciseX, e.wheel.preciseY}];
            binds.type    = action_type::changed;
        }
        return binds;
    }
};

struct touch_finger_mapper : public input_mapper<int64_t>
{
    input_mapping get_mapping(const SDL_Event& e)
    {
        input_mapping binds;
        if (e.type == SDL_FINGERDOWN)
        {
            binds.actions = bindings_[e.tfinger.fingerId];
            binds.type    = action_type::pressed;
        }
        if (e.type == SDL_FINGERMOTION)
        {
            binds.actions = bindings_[e.tfinger.fingerId];
            binds.type    = action_type::changed;
        }
        if (e.type == SDL_FINGERUP)
        {
            binds.actions = bindings_[e.tfinger.fingerId];
            binds.type    = action_type::released;
        }
        return binds;
    }
};

struct joystick_button_mapper : public input_mapper<std::pair<uint32_t, uint8_t>>
{
    input_mapping get_mapping(const SDL_Event& e)
    {
        input_mapping binds;
        if (e.type == SDL_JOYBUTTONDOWN)

        {
            binds.actions = bindings_[{e.jbutton.which, e.jbutton.button}];
            binds.type    = action_type::pressed;
        }
        if (e.type == SDL_JOYBUTTONUP)
        {
            binds.actions = bindings_[{e.jbutton.which, e.jbutton.button}];
            binds.type    = action_type::released;
        }
        return binds;
    }
};

struct event_mapper : public input_mapper<SDL_EventType>
{
    input_mapping get_mapping(const SDL_Event& e)
    {
        input_mapping binds;
        binds.actions = bindings_[static_cast<SDL_EventType>(e.type)];
        binds.type    = action_type::changed;
        return binds;
    }
};
