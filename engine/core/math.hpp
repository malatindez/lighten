#pragma once

#if defined(__clang__)
// TODO
#elif defined(__GNUC__) || defined(__GNUG__)
// TODO
#elif defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 6001)
#pragma warning(disable : 6287)
#pragma warning(disable : 26495)
#endif

#include "math/mat.hpp"
#include "math/ray.hpp"
#include "math/intersection.hpp"
#include "math/triangle.hpp"
#include "math/plane.hpp"
#include "math/box.hpp"
#include "math/quaternion.hpp"
#include "math/sphere.hpp"
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

#if defined(__clang__)
// TODO
#elif defined(__GNUC__) || defined(__GNUG__)
// TODO
#elif defined(_MSC_VER)
#pragma warning(pop)
#endif