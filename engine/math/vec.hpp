#pragma once
#include "mathpch.h"

namespace engine::math
{
    template <size_t L, Primitive T>
    struct vec;
    template <size_t L, Primitive T>
    struct rvec;
    namespace _detail
    {
        template <class T>
        struct is_vec : public std::false_type
        {
        };
        template <size_t size, Primitive T>
        struct is_vec<vec<size, T>> : public std::true_type
        {
        };
        template <size_t size, Primitive T>
        struct is_vec<rvec<size, T>> : public std::true_type
        {
        };
        template <class T>
        constexpr bool is_vec_v = is_vec<T>::value;
    } // namespace _detail

    template <class T>
    concept AnyVec = _detail::is_vec_v<T>;
} // namespace engine::math
#include "math/rvec2.hpp"
#include "math/rvec3.hpp"
#include "math/rvec4.hpp"
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
}; // namespace engine::math