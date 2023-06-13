#pragma once

namespace LunarYue::UI::API
{
    /**
     * Interface for any drawable class
     */
    class IDrawable
    {
    public:
        virtual void Draw() = 0;

    protected:
        virtual ~IDrawable() = default;
    };
} // namespace LunarYue::UI::API
