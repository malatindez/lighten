#pragma once
#include "math-common.hpp"
#include "rvec2.hpp"
#include "rvec3.hpp"
#include "rvec4.hpp"
#include "vec2.hpp"
#include "vec3.hpp"
#include "vec4.hpp"
#include "vecn.hpp"

namespace engine::core::math
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
}; // namespace engine::core::math