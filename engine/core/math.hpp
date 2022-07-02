#pragma once
#include "math/common.hpp"
#include "math/mat.hpp"
#include "math/mat_math.hpp"
#include "math/quaternion.hpp"
#include "math/vec.hpp"
#include "math/vec_math.hpp"

namespace engine::core::math
{
    template <class T>
    [[nodiscard]] constexpr vec<3, T> reflect(vec<3, T> const &normal,
                                              vec<3, T> const &dir)
    {
        return dir - 2.0f * dot(normal, dir) * normal;
    }
    template <class T>
    [[nodiscard]] constexpr vec<3, T> reflect_normal(vec<3, T> const &normal, vec<3, T> const &dir)
    {
        return normalize(dir - 2.0f * dot(normal, dir) * normal);
    }
    template <class T>
    [[nodiscard]] constexpr vec<3, T> reflect_normal_safe(vec<3, T> const &normal, vec<3, T> const &dir)
    {
        vec<3, T> t = dir - 2.0f * dot(normal, dir) * normal;
        float l = length(t);
        l = l * (l > 1e-6f) + 1 * (l <= 1e-6f);
        return t / l;
    }
} // namespace engine::core::math