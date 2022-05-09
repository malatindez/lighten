#pragma once
#include <cmath>
#include <concepts>
#include <istream>
#include <ostream>

template <typename T>
concept Primitive = std::floating_point<T> || std::integral<T>;

template <size_t L, Primitive T>
struct vec;

#include "math/vec2.hpp"
#include "math/vec3.hpp"
#include "math/vec4.hpp"

template <size_t size, Primitive T>
constexpr std::istream &operator>>(std::istream &is, vec<2, T> &vec)
{
    is >> vec.x >> vec.y;
    return is;
}
template <size_t size, Primitive T>
constexpr std::ostream &operator>>(std::ostream &os, vec<2, T> &vec)
{
    os << vec.x << " " << vec.y;
    return os;
}

template <size_t size, Primitive T, Primitive U>
constexpr T dot(vec<size, T> const &left, vec<size, U> const &right)
{
    T return_value = 0;
    for (size_t i = 0; i < size; i++)
    {
        return_value += left[i] * static_cast<T>(right[i]);
    }
    return return_value;
}
template <size_t size, Primitive T, Primitive U>
constexpr T angle(vec<size, T> const &left, vec<size, U> const &right)
{
    return std::acos(dot(left, right) / left.length() / right.length());
}

template <size_t size, Primitive T, Primitive U>
[[nodiscard]] constexpr vec<size, T> operator*(U const value, vec<size, T> const &vector)
{
    return vec<size, T>(vector) *= value;
}

template <size_t size, Primitive T, Primitive U>
[[nodiscard]] constexpr vec<size, T> operator+(vec<size, T> const &vector, U const value) noexcept
{
    return vec<size, T>(vector) += value;
}
template <size_t size, Primitive T, Primitive U>
[[nodiscard]] constexpr vec<size, T> operator-(vec<size, T> const &vector, U const value) noexcept
{
    return vec<size, T>(vector) -= value;
}
template <size_t size, Primitive T, Primitive U>
[[nodiscard]] constexpr vec<size, T> operator*(vec<size, T> const &vector, U const value) noexcept
{
    return vec<size, T>(vector) *= value;
}
template <size_t size, Primitive T, Primitive U>
[[nodiscard]] constexpr vec<size, T> operator/(vec<size, T> const &vector, U const value) noexcept
{
    return vec<size, T>(vector) /= value;
}
template <size_t size, Primitive T, Primitive U>
[[nodiscard]] constexpr vec<size, T> operator+(vec<size, T> const &left, vec<size, U> const &right) noexcept
{
    return vec<size, T>(left) += right;
}
template <size_t size, Primitive T, Primitive U>
[[nodiscard]] constexpr vec<size, T> operator-(vec<size, T> const &left, vec<size, U> const &right) noexcept
{
    return vec<size, T>(left) -= right;
}
template <size_t size, Primitive T, Primitive U>
[[nodiscard]] constexpr vec<size, T> operator*(vec<size, T> const &left, vec<size, U> const &right) noexcept
{
    return vec<size, T>(left) *= right;
}
template <size_t size, Primitive T, Primitive U>
[[nodiscard]] constexpr vec<size, T> operator/(vec<size, T> const &left, vec<size, U> const &right) noexcept
{
    return vec<size, T>(left) /= right;
}