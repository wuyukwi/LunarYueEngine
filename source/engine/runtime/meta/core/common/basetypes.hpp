#pragma once

#include <core/common_lib/basetypes.hpp>
#include <core/serialization/serialization.h>

namespace cereal
{
    template<typename Archive, typename T>
    inline void SERIALIZE_FUNCTION_NAME(Archive& ar, rect<T>& obj)
    {
        try_serialize(ar, cereal::make_nvp("left", obj.left));
        try_serialize(ar, cereal::make_nvp("top", obj.top));
        try_serialize(ar, cereal::make_nvp("right", obj.right));
        try_serialize(ar, cereal::make_nvp("bottom", obj.bottom));
    }

    template<typename Archive, typename T>
    inline void SERIALIZE_FUNCTION_NAME(Archive& ar, size<T>& obj)
    {
        try_serialize(ar, cereal::make_nvp("width", obj.width));
        try_serialize(ar, cereal::make_nvp("height", obj.height));
    }

    template<typename Archive, typename T>
    inline void SERIALIZE_FUNCTION_NAME(Archive& ar, point<T>& obj)
    {
        try_serialize(ar, cereal::make_nvp("x", obj.x));
        try_serialize(ar, cereal::make_nvp("y", obj.y));
    }

    template<typename Archive, typename T>
    inline void SERIALIZE_FUNCTION_NAME(Archive& ar, range<T>& obj)
    {
        try_serialize(ar, cereal::make_nvp("min", obj.min));
        try_serialize(ar, cereal::make_nvp("max", obj.max));
    }
} // namespace cereal
