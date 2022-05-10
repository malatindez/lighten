#pragma once
#include "vec4.hpp"
namespace engine::math
{
    template <Primitive T>
    constexpr void vec<4, T>::reset() noexcept
    {
        x = y = z = w = 0;
    }

    template <Primitive T>
    [[nodiscard]] constexpr vec<4, T> const &vec<4, T>::operator+() const noexcept
    {
        return *this;
    }
    template <Primitive T>
    [[nodiscard]] constexpr vec<4, T> vec<4, T>::operator-() const noexcept
    {
        return vec<4, T>(-x, -y, -z, -w);
    }

    template <Primitive T>
    template <Primitive U>
    constexpr vec<4, T> &vec<4, T>::operator+=(U const value) noexcept
    {
        x += value;
        y += value;
        z += value;
        w += value;
        return *this;
    }
    template <Primitive T>
    template <Primitive U>
    constexpr vec<4, T> &vec<4, T>::operator-=(U const value) noexcept
    {
        x -= value;
        y -= value;
        z -= value;
        w -= value;
        return *this;
    }
    template <Primitive T>
    template <Primitive U>
    constexpr vec<4, T> &vec<4, T>::operator*=(U const value) noexcept
    {
        x *= value;
        y *= value;
        z *= value;
        w *= value;
        return *this;
    }
    template <Primitive T>
    template <Primitive U>
    constexpr vec<4, T> &vec<4, T>::operator/=(U const value) noexcept
    {
        x /= value;
        y /= value;
        z /= value;
        w /= value;
        return *this;
    }
    template <Primitive T>
    template <Primitive U>
    constexpr vec<4, T> &vec<4, T>::operator+=(vec<4, U> const &other) noexcept
    {
        x += other.x;
        y += other.y;
        z += other.z;
        w += other.w;
        return *this;
    }
    template <Primitive T>
    template <Primitive U>
    constexpr vec<4, T> &vec<4, T>::operator-=(vec<4, U> const &other) noexcept
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        w -= other.w;
        return *this;
    }
    template <Primitive T>
    template <Primitive U>
    constexpr vec<4, T> &vec<4, T>::operator*=(vec<4, U> const &other) noexcept
    {
        x *= other.x;
        y *= other.y;
        z *= other.z;
        w *= other.w;
        return *this;
    }
    template <Primitive T>
    template <Primitive U>
    constexpr vec<4, T> &vec<4, T>::operator/=(vec<4, U> const &other) noexcept
    {
        x /= other.x;
        y /= other.y;
        z /= other.z;
        w /= other.w;
        return *this;
    }

    template <Primitive T>
    [[nodiscard]] constexpr T &vec<4, T>::operator[](size_t i)
    {
        assert(i < size);
        return data[i];
    }
    template <Primitive T>
    [[nodiscard]] constexpr T const &vec<4, T>::operator[](size_t i) const
    {
        assert(i < size);
        return data[i];
    }

}; // namespace engine::math
