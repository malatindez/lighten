#pragma once
#include "vec3.hpp"
namespace engine::math
{
    template <Primitive T>
    constexpr void vec<3, T>::reset() noexcept
    {
        x = y = z = 0;
    }

    template <Primitive T>
    [[nodiscard]] constexpr vec<3, T> const &vec<3, T>::operator+() const noexcept
    {
        return *this;
    }
    template <Primitive T>
    [[nodiscard]] constexpr vec<3, T> vec<3, T>::operator-() const noexcept
    {
        return vec<3, T>(-x, -y, -z);
    }

    template <Primitive T>
    template <Primitive U>
    constexpr vec<3, T> &vec<3, T>::operator+=(U const value) noexcept
    {
        x += value;
        y += value;
        z += value;
        return *this;
    }
    template <Primitive T>
    template <Primitive U>
    constexpr vec<3, T> &vec<3, T>::operator-=(U const value) noexcept
    {
        x -= value;
        y -= value;
        z -= value;
        return *this;
    }
    template <Primitive T>
    template <Primitive U>
    constexpr vec<3, T> &vec<3, T>::operator*=(U const value) noexcept
    {
        x *= value;
        y *= value;
        z *= value;
        return *this;
    }
    template <Primitive T>
    template <Primitive U>
    constexpr vec<3, T> &vec<3, T>::operator/=(U const value) noexcept
    {
        x /= value;
        y /= value;
        z /= value;
        return *this;
    }
    template <Primitive T>
    template <Primitive U>
    constexpr vec<3, T> &vec<3, T>::operator+=(vec<3, U> const &other) noexcept
    {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }
    template <Primitive T>
    template <Primitive U>
    constexpr vec<3, T> &vec<3, T>::operator-=(vec<3, U> const &other) noexcept
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }
    template <Primitive T>
    template <Primitive U>
    constexpr vec<3, T> &vec<3, T>::operator*=(vec<3, U> const &other) noexcept
    {
        x *= other.x;
        y *= other.y;
        z *= other.z;
        return *this;
    }
    template <Primitive T>
    template <Primitive U>
    constexpr vec<3, T> &vec<3, T>::operator/=(vec<3, U> const &other) noexcept
    {
        x /= other.x;
        y /= other.y;
        z /= other.z;
        return *this;
    }

    template <Primitive T>
    [[nodiscard]] constexpr T &vec<3, T>::operator[](size_t i)
    {
        assert(i < size);
        return data[i];
    }
    template <Primitive T>
    [[nodiscard]] constexpr T const &vec<3, T>::operator[](size_t i) const
    {
        assert(i < size);
        return data[i];
    }

    template <Primitive T, Primitive U>
    constexpr vec<3, T> cross(vec<3, T> const &left,
                              vec<3, U> const &right)
    {
        return vec<3, T>{left.y * right.z - left.z * right.y,
                         left.x * right.z - left.z * right.x,
                         left.x * right.y - left.y * right.x};
    }
}
