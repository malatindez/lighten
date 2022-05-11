#pragma once
#include "mat.hpp"
namespace engine::math
{
    template <size_t a, size_t b, size_t c, Primitive T, Primitive U>
    constexpr mat<a, c, T> operator*(mat<a, b, T> const &left, mat<b, c, U> const &right);
    template <size_t b, size_t c, Primitive T, Primitive U>
    constexpr vec<b, T> operator*(vec<b, T> const &left, mat<b, c, U> const &right);
    template <size_t a, size_t b, Primitive T>
    constexpr std::istream &operator>>(std::istream &is, mat<a, b, T> matrix);
    template <size_t a, size_t b, Primitive T>
    constexpr std::ostream &operator<<(std::ostream &os, mat<a, b, T> matrix);

    template <Primitive T>
    mat<4, 4, T> translate(mat<4, 4, T> const &matrix, vec<3, T> const &vec);

    template <Primitive T>
    mat<4, 4, T> rotate(mat<4, 4, T> const &matrix, T angle, vec<3, T> const &axis);

    template <Primitive T>
    mat<4, 4, T> scale(mat<4, 4, T> const &matrix, vec<3, T> const &scale);

    template <size_t a, size_t b, Primitive T>
    mat<b, a, T> transpose(mat<a, b, T> const &matrix);

    template <size_t a, Primitive T>
    constexpr T det(mat<a, a, T> const &matrix);

}
#include "mat_math.inl"