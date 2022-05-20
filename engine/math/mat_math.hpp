#pragma once
#include "mat.hpp"
namespace engine::math
{
    template <size_t a, size_t b, Primitive T>
    constexpr std::istream &operator>>(std::istream &is, mat<a, b, T> matrix);
    template <size_t a, size_t b, Primitive T>
    constexpr std::ostream &operator<<(std::ostream &os, mat<a, b, T> matrix);

    template <size_t a, size_t b, size_t c, Primitive T, Primitive U>
    [[nodiscard]] constexpr mat<a, c, T> operator*(mat<a, b, T> const &left,
                                                   mat<b, c, U> const &right);
    template <size_t b, size_t c, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<b, T> operator*(vec<b, T> const &left,
                                                mat<b, c, U> const &right);
    template <size_t a, size_t b, Primitive T, Primitive U>
    [[nodiscard]] constexpr mat<a, b, T> operator*(mat<a, b, T> const &left,
                                                   U const right);
    template <size_t a, size_t b, Primitive T, Primitive U>
    [[nodiscard]] constexpr mat<a, b, T> operator*(U const left,
                                                   mat<a, b, T> const &right);

    template <size_t a, size_t b, Primitive T>
    constexpr mat<b, a, T> transpose(mat<a, b, T> const &matrix);

    template <size_t a, Primitive T>
    constexpr T det(mat<a, a, T> const &matrix);

    template <size_t a, Primitive T>
    constexpr mat<a, a, T> adjugate(mat<a, a, T> const &m);

    template <size_t a, Primitive T>
    constexpr mat<a, a, T> inverse(mat<a, a, T> const &m);

    template <Primitive T>
    constexpr mat<4, 4, T> translate(mat<4, 4, T> const &matrix, vec<3, T> const &vec);

    template <Primitive T>
    constexpr mat<4, 4, T> rotate(mat<4, 4, T> const &matrix, T angle, vec<3, T> const &axis);

    template <Primitive T>
    constexpr mat<4, 4, T> scale(mat<4, 4, T> const &matrix, vec<3, T> const &scale);

    template <Primitive T>
    constexpr mat<4, 4, T> lookAt(vec<3, T> const &eye, vec<3, T> const &center,
                        vec<3, T> const &up);

    template <Primitive T>
    constexpr mat<4, 4, T> perspective(T fov_y, T aspect_ratio, T z_near, T z_far);

    template <Primitive T>
    constexpr void invert_orthonormal(mat<4, 4, T> const&src, mat<4, 4, T>& dst);
    template <Primitive T>
    constexpr void invert_orthogonal(mat<4, 4, T> const&src, mat<4, 4, T>& dst);
    template <Primitive T>
    constexpr mat<4, 4, T> invert_orthonormal(mat<4, 4, T> const&src);
    template <Primitive T>
    constexpr mat<4, 4, T> invert_orthogonal(mat<4, 4, T> const&src);

} // namespace engine::math
#include "mat_math.inl"