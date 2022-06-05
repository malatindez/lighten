#pragma once
#include "math/mat.hpp"
#include "math/mat_math.hpp"
#include "math/common.hpp"
#include "math/quaternion.hpp"
#include "math/vec.hpp"
#include "math/vec_math.hpp"

namespace engine::core::math
{
    template <class T>
    [[nodiscard]] constexpr vec<3, T> reflect(vec<3, T> const &normal,
                                                    vec<3, T> const &dir)
    {
        return 2.0f * dot(normal, dir) * (normal - dir);
    }
    template <class T>
    [[nodiscard]] constexpr vec<3, T> reflect_normal(vec<3, T> const &normal, vec<3, T> const &dir)
    {
        return normalize(2.0f * dot(normal, dir) * (normal - dir));
    }
    template <class T>
    [[nodiscard]] constexpr vec<3, T> reflect_normal_safe(vec<3, T> const &normal, vec<3, T> const &dir)
    {
        vec<3, T> t = 2.0f * dot(normal, dir) * (normal - dir);
        if (length(t) <= 1e-6f)
        {
            return t;
        }
        return normalize(t);
    }
} // namespace engine::core::math