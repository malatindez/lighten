#pragma once
#include "math/mat.hpp"
#include "math/mat_math.hpp"
#include "math/mathpch.h"
#include "math/quaternion.hpp"
#include "math/vec.hpp"
#include "math/vec_math.hpp"

namespace engine::math
{
    template <class T>
    [[nodiscard]] constexpr math::vec<3, T> reflect(math::vec<3, T> const &normal,
                                                    math::vec<3, T> const &dir)
    {
        return 2.0f * math::dot(normal, dir) * (normal - dir);
    }
    template <class T>
    [[nodiscard]] constexpr math::vec<3, T>
    reflect_normal(math::vec<3, T> const &normal, math::vec<3, T> const &dir)
    {
        return normalize(2.0f * math::dot(normal, dir) * (normal - dir));
    }
    template <class T>
    [[nodiscard]] constexpr math::vec<3, T>
    reflect_normal_safe(math::vec<3, T> const &normal, math::vec<3, T> const &dir)
    {
        math::vec<3, T> t = 2.0f * math::dot(normal, dir) * (normal - dir);
        if (math::length(t) <= 1e-6f)
        {
            return t;
        }
        return normalize(t);
    }
} // namespace engine::math