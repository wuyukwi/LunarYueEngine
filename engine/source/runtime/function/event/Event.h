#pragma once

#include <functional>

namespace LunarYue
{
    /**
     * The ID of a listener (Registered callback).
     * This value is needed to remove a listener from an event
     */
    using ListenerID = uint64_t;

    /**
     * A simple event that contains a set of function callbacks. These functions will be called on invoke
     */
    template<class... ArgTypes>
    class Event
    {
    public:
        /**
         * Simple shortcut for a generic function without return value
         */
        using Callback = std::function<void(ArgTypes...)>;

        /**
         * Add a function callback to this event
         * Also return the ID of the new listener (You should store the returned ID if you want to remove the listener later)
         * @param p_call
         */
        ListenerID AddListener(Callback callback);

        /**
         * Add a function callback to this event
         * Also return the ID of the new listener (You should store the returned ID if you want to remove the listener later)
         * @param p_call
         */
        ListenerID operator+=(Callback callback);

        /**
         * Remove a function callback to this event using a Listener (Created when calling AddListener)
         * @param p_listener
         */
        bool RemoveListener(ListenerID listenerID);

        /**
         * Remove a function callback to this event using a Listener (Created when calling AddListener)
         * @param p_listener
         */
        bool operator-=(ListenerID listenerID);

        /**
         * Remove every listeners to this event
         */
        void RemoveAllListeners();

        /**
         * Return the number of callback registered
         */
        uint64_t GetListenerCount();

        /**
         * Call every callbacks attached to this event
         * @param p_args (Variadic)
         */
        void Invoke(ArgTypes... p_args);

    private:
        std::unordered_map<ListenerID, Callback> m_callbacks;
        ListenerID                               m_availableListenerID = 0;
    };

    template<class... ArgTypes>
    ListenerID Event<ArgTypes...>::AddListener(Callback callback)
    {
        ListenerID listenerID = m_availableListenerID++;
        m_callbacks.emplace(listenerID, callback);
        return listenerID;
    }

    template<class... ArgTypes>
    ListenerID Event<ArgTypes...>::operator+=(Callback callback)
    {
        return AddListener(callback);
    }

    template<class... ArgTypes>
    bool Event<ArgTypes...>::RemoveListener(ListenerID listenerID)
    {
        return m_callbacks.erase(listenerID) != 0;
    }

    template<class... ArgTypes>
    bool Event<ArgTypes...>::operator-=(ListenerID listenerID)
    {
        return RemoveListener(listenerID);
    }

    template<class... ArgTypes>
    void Event<ArgTypes...>::RemoveAllListeners()
    {
        m_callbacks.clear();
    }

    template<class... ArgTypes>
    uint64_t Event<ArgTypes...>::GetListenerCount()
    {
        return m_callbacks.size();
    }

    template<class... ArgTypes>
    void Event<ArgTypes...>::Invoke(ArgTypes... p_args)
    {
        for (auto const& [key, value] : m_callbacks)
            value(p_args...);
    }
} // namespace LunarYue
