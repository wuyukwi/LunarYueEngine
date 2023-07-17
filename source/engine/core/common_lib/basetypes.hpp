#pragma once

#include <array>
#include <chrono>
#include <cstdint>
#include <type_traits>

template<typename T>
struct range
{
    using value_type = T;
    range() = default;
    range(T _min, T _max) : min(_min), max(_max)
    {
    }

    template<typename U, typename = typename std::enable_if<std::is_convertible<U, T>::value, void>::type>
    explicit range(const std::array<U, 2>& data) : min(data[0])
                                                 , max(data[1])
    {
    }
    T min = 0;
    T max = 0;

    inline bool contains(const T& val) const
    {
        return min <= val && val <= max;
    }

    inline bool operator==(const range& b) const
    {
        return (min == b.Min && max == b.Max);
    }
    inline bool operator!=(const range& b) const
    {
        return (min != b.Min || max != b.Max);
    }

}; // End Struct range<Float>

template<typename T>
struct size
{
    using value_type = T;
    size() = default;
    size(T _width, T _height) : width(_width), height(_height)
    {
    }

    template<typename U, typename = typename std::enable_if<std::is_convertible<U, T>::value, void>::type>
    explicit size(const std::array<U, 2>& data) : width(data[0])
                                                , height(data[1])
    {
    }
    T width = 0;
    T height = 0;

    // Inline operators
    inline bool operator==(const size& b) const
    {
        return (width == b.width && height == b.height);
    }
    inline bool operator!=(const size& b) const
    {
        return (width != b.width || height != b.height);
    }
    inline bool operator<(const size& b) const
    {
        if(width != b.width)
        {
            return (width < b.width);
        }
        return (height < b.height);
    }
    inline bool operator>(const size& b) const
    {
        if(width < b.width)
        {
            return false;
        }
        if(width > b.width)
        {
            return true;
        }
        return (height > b.height);
    }

    inline bool is_valid() const
    {
        return width != 0 && height != 0;
    }

}; // End Struct TSize

template<typename T>
struct point
{
    using value_type = T;
    point() = default;

    point(T _x, T _y) : x(_x), y(_y)
    {
    }
    template<typename U, typename = typename std::enable_if<std::is_convertible<U, T>::value, void>::type>
    explicit point(const std::array<U, 2>& data) : x(data[0])
                                                 , y(data[1])
    {
    }
    T x = 0;
    T y = 0;

    // Inline operators
    inline bool operator==(const point& b) const
    {
        return (x == b.x && y == b.y);
    }
    inline bool operator!=(const point& b) const
    {
        return (x != b.x || y != b.y);
    }

}; // End Struct point

template<typename T>
struct rect
{
    using value_type = T;
    rect() = default;
    rect(T _left, T _top, T _right, T _bottom) : left(_left), top(_top), right(_right), bottom(_bottom)
    {
    }
    T left = 0;
    T top = 0;
    T right = 0;
    T bottom = 0;

    inline T width() const
    {
        return right - left;
    }
    inline T height() const
    {
        return bottom - top;
    }
    inline bool empty() const
    {
        return (left == 0 && right == 0 && top == 0 && bottom == 0);
    }

    template<typename T1 = T>
    inline size<T1> size() const
    {
        return size<T1>(right - left /*+ 1*/, bottom - top /*+ 1*/);
    }

    template<typename T1 = T>
    inline point<T1> center() const
    {
        return {left + width() / 2, top + height() / 2};
    }

    template<typename T1 = T>
    inline point<T1> tl() const
    {
        return {left, top};
    }
    template<typename T1 = T>
    inline point<T1> tr() const
    {
        return {right, top};
    }

    template<typename T1 = T>
    inline point<T1> bl() const
    {
        return {left, bottom};
    }
    template<typename T1 = T>
    inline point<T1> br() const
    {
        return {right, bottom};
    }

    inline bool contains(const point<T>& p) const
    {
        return (p.x >= left && p.x <= right && p.y >= top && p.y <= bottom);
    }

    inline static rect intersect(const rect& a, const rect& b)
    {
        rect c(std::max(a.left, b.left),
               std::max(a.top, b.top),
               std::min(a.right, b.right),
               std::min(a.bottom, b.bottom));

        // If no intersection occurred, just return an empty rectangle
        if(c.left > c.right || c.top > c.bottom)
        {
            return rect();
        }

        return c;
    }

    inline bool operator==(const rect& b) const
    {
        return (left == b.left && top == b.top && right == b.right && bottom == b.bottom);
    }
    inline bool operator!=(const rect& b) const
    {
        return (left != b.left || top != b.top || right != b.right || bottom != b.bottom);
    }
    inline rect& operator+=(const point<T>& p)
    {
        left += p.x;
        right += p.x;
        top += p.y;
        bottom += p.y;
        return *this;
    }
    inline rect& operator-=(const point<T>& p)
    {
        left -= p.x;
        right -= p.x;
        top -= p.y;
        bottom -= p.y;
        return *this;
    }
    inline bool operator<(const rect& b) const
    {
        T r = left - b.left;
        if(r)
        {
            return (r < 0);
        }
        r = top - b.top;
        if(r)
        {
            return (r < 0);
        }
        r = right - b.right;
        if(r)
        {
            return (r < 0);
        }
        r = bottom - b.bottom;
        if(r)
        {
            return (r < 0);
        }
        return false;
    }
    inline bool operator>(const rect& b) const
    {
        T r = left - b.left;
        if(r)
        {
            return (r > 0);
        }
        r = top - b.top;
        if(r)
        {
            return (r > 0);
        }
        r = right - b.right;
        if(r)
        {
            return (r > 0);
        }
        r = bottom - b.bottom;
        if(r)
        {
            return (r > 0);
        }
        return false;
    }

    inline static rect inflate(const rect& rc, T x, T y)
    {
        return rect(rc.left - x, rc.top - y, rc.right + x, rc.bottom + y);
    }
};

using irange8_t = range<std::int8_t>;
using irange16_t = range<std::int16_t>;
using irange32_t = range<std::int32_t>;
using irange64_t = range<std::int64_t>;
using urange8_t = range<std::uint8_t>;
using urange16_t = range<std::uint16_t>;
using urange32_t = range<std::uint32_t>;
using urange64_t = range<std::uint64_t>;
using frange_t = range<float>;
using drange_t = range<double>;

using isize8_t = size<std::int8_t>;
using isize16_t = size<std::int16_t>;
using isize32_t = size<std::int32_t>;
using isize64_t = size<std::int64_t>;
using usize8_t = size<std::uint8_t>;
using usize16_t = size<std::uint16_t>;
using usize32_t = size<std::uint32_t>;
using usize64_t = size<std::uint64_t>;
using fsize_t = size<float>;
using dsize_t = size<double>;

using ipoint8_t = point<std::int8_t>;
using ipoint16_t = point<std::int16_t>;
using ipoint32_t = point<std::int32_t>;
using ipoint64_t = point<std::int64_t>;
using upoint8_t = point<std::uint8_t>;
using upoint16_t = point<std::uint16_t>;
using upoint32_t = point<std::uint32_t>;
using upoint64_t = point<std::uint64_t>;
using fpoint_t = point<float>;
using dpoint_t = point<double>;

using irect8_t = rect<std::int8_t>;
using irect16_t = rect<std::int16_t>;
using irect32_t = rect<std::int32_t>;
using irect64_t = rect<std::int64_t>;
using urect8_t = rect<std::uint8_t>;
using urect16_t = rect<std::uint16_t>;
using urect32_t = rect<std::uint32_t>;
using urect64_t = rect<std::uint64_t>;
using frect_t = rect<float>;
using drect_t = rect<double>;

using delta_t = std::chrono::duration<float>;
