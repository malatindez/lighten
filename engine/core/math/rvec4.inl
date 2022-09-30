#pragma once
#include "rvec4.hpp"
namespace engine::core::math::_detail
{
    template <Primitive T>
    constexpr void rvec<4, T>::reset() noexcept
    {
        x = y = z = w = 0;
    }

    template <Primitive T>
    [[nodiscard]] constexpr rvec<4, T> const &rvec<4, T>::operator+() const noexcept
    {
        return *this;
    }
    template <Primitive T>
    [[nodiscard]] constexpr vec<4, T> rvec<4, T>::operator-() const noexcept
    {
        return vec<4, T>(-x, -y, -z, -w);
    }

    template <Primitive T>
    template <Primitive U>
    constexpr rvec<4, T> &rvec<4, T>::operator+=(U const value) noexcept
    {
        x = static_cast<T>(x + value);
        y = static_cast<T>(y + value);
        z = static_cast<T>(z + value);
        w = static_cast<T>(w + value);
        return *this;
    }
    template <Primitive T>
    template <Primitive U>
    constexpr rvec<4, T> &rvec<4, T>::operator-=(U const value) noexcept
    {
        x = static_cast<T>(x - value);
        y = static_cast<T>(y - value);
        z = static_cast<T>(z - value);
        w = static_cast<T>(w - value);
        return *this;
    }
    template <Primitive T>
    template <Primitive U>
    constexpr rvec<4, T> &rvec<4, T>::operator*=(U const value) noexcept
    {
        x = static_cast<T>(x * value);
        y = static_cast<T>(y * value);
        z = static_cast<T>(z * value);
        w = static_cast<T>(w * value);
        return *this;
    }
    template <Primitive T>
    template <Primitive U>
    constexpr rvec<4, T> &rvec<4, T>::operator/=(U const value) noexcept
    {
        x = static_cast<T>(x / value);
        y = static_cast<T>(y / value);
        z = static_cast<T>(z / value);
        w = static_cast<T>(w / value);
        return *this;
    }
    template <Primitive T>
    template <Primitive U>
    constexpr rvec<4, T> &rvec<4, T>::operator%=(U const value) noexcept
    {
        x = static_cast<T>(x % value);
        y = static_cast<T>(y % value);
        z = static_cast<T>(z % value);
        w = static_cast<T>(w % value);
        return *this;
    }
    template <Primitive T>
    template <AnyVec U>
    constexpr rvec<4, T> &rvec<4, T>::operator+=(U const &other) noexcept requires(size == U::size)
    {
        x = static_cast<T>(x + other.x);
        y = static_cast<T>(y + other.y);
        z = static_cast<T>(z + other.z);
        w = static_cast<T>(w + other.w);
        return *this;
    }
    template <Primitive T>
    template <AnyVec U>
    constexpr rvec<4, T> &rvec<4, T>::operator-=(U const &other) noexcept requires(size == U::size)
    {
        x = static_cast<T>(x - other.x);
        y = static_cast<T>(y - other.y);
        z = static_cast<T>(z - other.z);
        w = static_cast<T>(w - other.w);
        return *this;
    }
    template <Primitive T>
    template <AnyVec U>
    constexpr rvec<4, T> &rvec<4, T>::operator*=(U const &other) noexcept requires(size == U::size)
    {
        x = static_cast<T>(x * other.x);
        y = static_cast<T>(y * other.y);
        z = static_cast<T>(z * other.z);
        w = static_cast<T>(w * other.w);
        return *this;
    }
    template <Primitive T>
    template <AnyVec U>
    constexpr rvec<4, T> &rvec<4, T>::operator/=(U const &other) noexcept requires(size == U::size)
    {
        x = static_cast<T>(x / other.x);
        y = static_cast<T>(y / other.y);
        z = static_cast<T>(z / other.z);
        w = static_cast<T>(w / other.w);
        return *this;
    }
    template <Primitive T>
    template <AnyVec U>
    constexpr rvec<4, T> &rvec<4, T>::operator%=(U const &other) noexcept requires(size == U::size)
    {
        x = static_cast<T>(x % other.x);
        y = static_cast<T>(y % other.y);
        z = static_cast<T>(z % other.z);
        w = static_cast<T>(w % other.w);
        return *this;
    }
    template <Primitive T>
    [[nodiscard]] constexpr T &rvec<4, T>::operator[](size_t i)
    {
        assert(i < size);
        return data[i];
    }
    template <Primitive T>
    [[nodiscard]] constexpr T const &rvec<4, T>::operator[](size_t i) const
    {
        assert(i < size);
        return data[i];
    }

    template <Primitive T>
    template <size_t n>
    [[nodiscard]] constexpr rvec<n, T> rvec<4, T>::as_rvec() noexcept requires(n >= 2 && n <= size)
    {
        return rvec<n, T>{*this};
    }

    template <Primitive T>
    template <size_t n, Primitive U>
    [[nodiscard]] constexpr vec<n, std::remove_const_t<U>> rvec<4, T>::as_vec() const noexcept requires(n >= 2 && n <= size)
    {
        vec<n, std::remove_const_t<U>> rv;
        for (size_t i = 0; i < n; i++)
        {
            rv.data[i] = static_cast<U>(data[i]);
        }
        return rv;
    }

    template <Primitive T>
    template <size_t n>
    [[nodiscard]] constexpr rvec<n, const T> rvec<4, T>::as_crvec() const noexcept requires(n >= 2 && n <= size)
    {
        return rvec<n, const T>{*this};
    }
}; // namespace engine::core::math