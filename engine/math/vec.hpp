#pragma once
#include <array>
#include <assert.h>
#include <cmath>
#include <concepts>
#include <istream>
#include <ostream>

template <typename T>
concept Primitive = std::floating_point<T> || std::integral<T>;
namespace engine::math
{
    template <size_t L, Primitive T>
    struct vec;
}
#include "math/vec2.hpp"
#include "math/vec3.hpp"
#include "math/vec4.hpp"
#include "math/vecn.hpp"

namespace engine::math
{
    using vec2 = vec<2, float>;
    using dvec2 = vec<2, double>;
    using ivec2 = vec<2, int>;
    using uivec2 = vec<2, unsigned int>;

    using vec3 = vec<3, float>;
    using dvec3 = vec<3, double>;
    using ivec3 = vec<3, int>;
    using uivec3 = vec<3, unsigned int>;

    using vec4 = vec<4, float>;
    using dvec4 = vec<4, double>;
    using ivec4 = vec<4, int>;
    using uivec4 = vec<4, unsigned int>;
}; // namespace engine::math