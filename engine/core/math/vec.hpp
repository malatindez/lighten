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

    using rvec2 = rvec<2, float>;
    using rdvec2 = rvec<2, double>;
    using rivec2 = rvec<2, int>;
    using ruivec2 = rvec<2, unsigned int>;

    using rvec3 = rvec<3, float>;
    using rdvec3 = rvec<3, double>;
    using rivec3 = rvec<3, int>;
    using ruivec3 = rvec<3, unsigned int>;

    using rvec4 = rvec<4, float>;
    using rdvec4 = rvec<4, double>;
    using rivec4 = rvec<4, int>;
    using ruivec4 = rvec<4, unsigned int>;

    using crvec2 = rvec<2, const float>;
    using crdvec2 = rvec<2, const double>;
    using crivec2 = rvec<2, const int>;
    using cruivec2 = rvec<2, const unsigned int>;

    using crvec3 = rvec<3, const float>;
    using crdvec3 = rvec<3, const double>;
    using crivec3 = rvec<3, const int>;
    using cruivec3 = rvec<3, const unsigned int>;

    using crvec4 = rvec<4, const float>;
    using crdvec4 = rvec<4, const double>;
    using crivec4 = rvec<4, const int>;
    using cruivec4 = rvec<4, const unsigned int>;

    static_assert(_detail::is_vec_v<vec2>);
    static_assert(_detail::is_vec_v<rvec2>);
    static_assert(_detail::is_vec_v<vec<32, uint64_t>>);
    static_assert(_detail::is_vec_v<rvec<32, uint64_t>>);
    static_assert(_detail::is_default_vec_v<vec2>);
    static_assert(!_detail::is_default_vec_v<rvec2>);
    static_assert(!_detail::is_reference_vec_v<vec2>);
    static_assert(_detail::is_reference_vec_v<rvec2>);
    static_assert(_detail::is_vec_v<vec<32, uint64_t>>);
    static_assert(_detail::is_vec_v<rvec<32, uint64_t>>);
    static_assert(_detail::is_default_vec_v<vec<32, uint64_t>>);
    static_assert(!_detail::is_default_vec_v<rvec<32, uint64_t>>);
    static_assert(!_detail::is_reference_vec_v<vec<32, uint64_t>>);
    static_assert(_detail::is_reference_vec_v<rvec<32, uint64_t>>);
}; // namespace engine::core::math