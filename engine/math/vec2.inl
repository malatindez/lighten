#pragma once
#include "vec2.hpp"
namespace engine::math
{
    template <Primitive T>
    constexpr void vec<2, T>::reset() noexcept
    {
        x = y = 0;
    }

    template <Primitive T>
    [[nodiscard]] constexpr vec<2, T> const &vec<2, T>::operator+() const noexcept
    {
        return *this;
    }

    template <Primitive T>
    [[nodiscard]] constexpr vec<2, T> vec<2, T>::operator-() const noexcept
    {
        return vec<2, T>(-static_cast<T>(x), -static_cast<T>(y));
    }

    template <Primitive T>
    template <Primitive U>
    constexpr vec<2, T> &vec<2, T>::operator+=(U const value) noexcept
    {
        x += value;
        y += value;
        return *this;
    }
    template <Primitive T>
    template <Primitive U>
    constexpr vec<2, T> &vec<2, T>::operator-=(U const value) noexcept
    {
        x -= value;
        y -= value;
        return *this;
    }
    template <Primitive T>
    template <Primitive U>
    constexpr vec<2, T> &vec<2, T>::operator*=(U const value) noexcept
    {
        x *= value;
        y *= value;
        return *this;
    }
    template <Primitive T>
    template <Primitive U>
    constexpr vec<2, T> &vec<2, T>::operator/=(U const value) noexcept
    {
        x /= value;
        y /= value;
        return *this;
    }
    template <Primitive T>
    template <Primitive U>
    constexpr vec<2, T> &vec<2, T>::operator+=(vec<2, U> const &other) noexcept
    {
        x += other.x;
        y += other.y;
        return *this;
    }
    template <Primitive T>
    template <Primitive U>
    constexpr vec<2, T> &vec<2, T>::operator-=(vec<2, U> const &other) noexcept
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }
    template <Primitive T>
    template <Primitive U>
    constexpr vec<2, T> &vec<2, T>::operator*=(vec<2, U> const &other) noexcept
    {
        x *= other.x;
        y *= other.y;
        return *this;
    }
    template <Primitive T>
    template <Primitive U>
    constexpr vec<2, T> &vec<2, T>::operator/=(vec<2, U> const &other) noexcept
    {
        x /= other.x;
        y /= other.y;
        return *this;
    }
    template <Primitive T>
    [[nodiscard]] constexpr T &vec<2, T>::operator[](size_t i)
    {
        assert(i < size);
        return data[i];
    }
    template <Primitive T>
    [[nodiscard]] constexpr T const &vec<2, T>::operator[](size_t i) const
    {
        assert(i < size);
        return data[i];
    }
}
