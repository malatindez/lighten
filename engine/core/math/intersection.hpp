#pragma once
#include "../math.hpp"
#pragma warning(push)
#pragma warning(disable : 26495)
namespace engine::core::math
{
    struct Intersection
    {
        float t;
        vec3 point;
        vec3 normal;

        constexpr Intersection() = default;
        constexpr void reset() { t = std::numeric_limits<float>::infinity(); }
        inline bool exists() const { return std::isfinite(t); }
        static constexpr Intersection infinite()
        {
            Intersection inf;
            inf.reset();
            return inf;
        }
    };
} // namespace engine::core::math
#pragma warning(pop)