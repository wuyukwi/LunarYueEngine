#pragma once

namespace LunarYue
{
    class DebugDrawManager
    {
    public:
        DebugDrawManager() = default;
        void initialize();
        void destory();
        void clear();
        void tick(float delta_time);

        ~DebugDrawManager() { destory(); }

    private:
    };

} // namespace LunarYue
