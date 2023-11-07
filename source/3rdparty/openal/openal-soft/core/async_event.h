#ifndef CORE_EVENT_H
#define CORE_EVENT_H

#include <stdint.h>
#include <variant>

#include "almalloc.h"

struct EffectState;

using uint = unsigned int;


enum class AsyncEnableBits : uint8_t {
    SourceState,
    BufferCompleted,
    Disconnected,
    Count
};


enum class AsyncSrcState : uint8_t {
    Reset,
    Stop,
    Play,
    Pause
};

using AsyncKillThread = std::monostate;

struct AsyncSourceStateEvent {
    uint mId;
    AsyncSrcState mState;
};

struct AsyncBufferCompleteEvent {
    uint mId;
    uint mCount;
};

struct AsyncDisconnectEvent {
    char msg[244];
};

struct AsyncEffectReleaseEvent {
    EffectState *mEffectState;
};

using AsyncEvent = std::variant<AsyncKillThread,
        AsyncSourceStateEvent,
        AsyncBufferCompleteEvent,
        AsyncEffectReleaseEvent,
        AsyncDisconnectEvent>;

template<typename T, typename ...Args>
auto &InitAsyncEvent(std::byte *evtbuf, Args&& ...args)
{
    auto *evt = al::construct_at(reinterpret_cast<AsyncEvent*>(evtbuf), std::in_place_type<T>,
        std::forward<Args>(args)...);
    return std::get<T>(*evt);
}

#endif
