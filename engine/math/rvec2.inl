#pragma once
#include "rvec2.hpp"
namespace engine::core::math
{
    template <Primitive T>
    constexpr void rvec<2, T>::reset() noexcept { x = y = 0; }

    template <Primitive T>
    [[nodiscard]] constexpr rvec<2, T> const &rvec<2, T>::operator+() const noexcept
    {
        return *this;
    }

    template <Primitive T>
    [[nodiscard]] constexpr vec<2, T> rvec<2, T>::operator-() const noexcept
    {
        return vec<2, T>(-static_cast<T>(x), -static_cast<T>(y));
    }

    template <Primitive T>
    template <Primitive U>
    constexpr rvec<2, T> &rvec<2, T>::operator+=(U const value) noexcept
    {
        x += static_cast<T>(value);
        y += static_cast<T>(value);
        return *this;
    }
    template <Primitive T>
    template <Primitive U>
    constexpr rvec<2, T> &rvec<2, T>::operator-=(U const value) noexcept
    {
        x -= static_cast<T>(value);
        y -= static_cast<T>(value);
        return *this;
    }
    template <Primitive T>
    template <Primitive U>
    constexpr rvec<2, T> &rvec<2, T>::operator*=(U const value) noexcept
    {
        x *= static_cast<T>(value);
        y *= static_cast<T>(value);
        return *this;
    }
    template <Primitive T>
    template <Primitive U>
    constexpr rvec<2, T> &rvec<2, T>::operator/=(U const value) noexcept
    {
        x /= static_cast<T>(value);
        y /= static_cast<T>(value);
        return *this;
    }
    template <Primitive T>
    template <Primitive U>
    constexpr rvec<2, T> &rvec<2, T>::operator%=(U const value) noexcept
    {
        x %= static_cast<T>(value);
        y %= static_cast<T>(value);
        return *this;
    }
    template <Primitive T>
    template <AnyVec U>
    constexpr rvec<2, T> &rvec<2, T>::operator+=(U const &other) noexcept requires(size == U::size)
    {
        x += other.x;
        y += other.y;
        return *this;
    }
    template <Primitive T>
    template <AnyVec U>
    constexpr rvec<2, T> &rvec<2, T>::operator-=(U const &other) noexcept requires(size == U::size)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }
    template <Primitive T>
    template <AnyVec U>
    constexpr rvec<2, T> &rvec<2, T>::operator*=(U const &other) noexcept requires(size == U::size)
    {
        x *= other.x;
        y *= other.y;
        return *this;
    }
    template <Primitive T>
    template <AnyVec U>
    constexpr rvec<2, T> &rvec<2, T>::operator/=(U const &other) noexcept requires(size == U::size)
    {
        x /= other.x;
        y /= other.y;
        return *this;
    }
    template <Primitive T>
    template <AnyVec U>
    constexpr rvec<2, T> &rvec<2, T>::operator%=(U const &other) noexcept requires(size == U::size)
    {
        x %= other.x;
        y %= other.y;
        return *this;
    }
    template <Primitive T>
    [[nodiscard]] constexpr T &rvec<2, T>::operator[](size_t i)
    {
        assert(i < size);
        return data[i];
    }
    template <Primitive T>
    [[nodiscard]] constexpr T const &rvec<2, T>::operator[](size_t i) const
    {
        assert(i < size);
        return data[i];
    }

} // namespace engine::core::math
