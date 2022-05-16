#pragma once
#include "math.hpp"
namespace engine::math
{
    struct Intersection
    {
        float t;
        vec3 point;

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
}