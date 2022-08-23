#pragma once
#include "rvec3.hpp"
namespace engine::core::math::_detail
{
    template <Primitive T>
    constexpr void rvec<3, T>::reset() noexcept
    {
        x = y = z = 0;
    }

    template <Primitive T>
    [[nodiscard]] constexpr rvec<3, T> const &rvec<3, T>::operator+() const noexcept
    {
        return *this;
    }
    template <Primitive T>
    [[nodiscard]] constexpr vec<3, T> rvec<3, T>::operator-() const noexcept
    {
        return vec<3, T>(-x, -y, -z);
    }

    template <Primitive T>
    template <Primitive U>
    constexpr rvec<3, T> &rvec<3, T>::operator+=(U const value) noexcept
    {
        x = static_cast<T>(x + value);
        y = static_cast<T>(y + value);
        z = static_cast<T>(z + value);
        return *this;
    }
    template <Primitive T>
    template <Primitive U>
    constexpr rvec<3, T> &rvec<3, T>::operator-=(U const value) noexcept
    {
        x = static_cast<T>(x - value);
        y = static_cast<T>(y - value);
        z = static_cast<T>(z - value);
        return *this;
    }
    template <Primitive T>
    template <Primitive U>
    constexpr rvec<3, T> &rvec<3, T>::operator*=(U const value) noexcept
    {
        x = static_cast<T>(x * value);
        y = static_cast<T>(y * value);
        z = static_cast<T>(z * value);
        return *this;
    }
    template <Primitive T>
    template <Primitive U>
    constexpr rvec<3, T> &rvec<3, T>::operator/=(U const value) noexcept
    {
        x = static_cast<T>(x / value);
        y = static_cast<T>(y / value);
        z = static_cast<T>(z / value);
        return *this;
    }
    template <Primitive T>
    template <Primitive U>
    constexpr rvec<3, T> &rvec<3, T>::operator%=(U const value) noexcept
    {
        x = static_cast<T>(x % value);
        y = static_cast<T>(y % value);
        z = static_cast<T>(z % value);
        return *this;
    }

    template <Primitive T>
    template <AnyVec U>
    constexpr rvec<3, T> &rvec<3, T>::operator+=(U const &other) noexcept requires(size == U::size)
    {
        x = static_cast<T>(x + other.x);
        y = static_cast<T>(y + other.y);
        z = static_cast<T>(z + other.z);
        return *this;
    }
    template <Primitive T>
    template <AnyVec U>
    constexpr rvec<3, T> &rvec<3, T>::operator-=(U const &other) noexcept requires(size == U::size)
    {
        x = static_cast<T>(x - other.x);
        y = static_cast<T>(y - other.y);
        z = static_cast<T>(z - other.z);
        return *this;
    }
    template <Primitive T>
    template <AnyVec U>
    constexpr rvec<3, T> &rvec<3, T>::operator*=(U const &other) noexcept requires(size == U::size)
    {
        x = static_cast<T>(x * other.x);
        y = static_cast<T>(y * other.y);
        z = static_cast<T>(z * other.z);
        return *this;
    }
    template <Primitive T>
    template <AnyVec U>
    constexpr rvec<3, T> &rvec<3, T>::operator/=(U const &other) noexcept requires(size == U::size)
    {
        x = static_cast<T>(x / other.x);
        y = static_cast<T>(y / other.y);
        z = static_cast<T>(z / other.z);
        return *this;
    }

    template <Primitive T>
    template <AnyVec U>
    constexpr rvec<3, T> &rvec<3, T>::operator%=(U const &other) noexcept requires(size == U::size)
    {
        x = static_cast<T>(x % other.x);
        y = static_cast<T>(y % other.y);
        z = static_cast<T>(z % other.z);
        return *this;
    }
    template <Primitive T>
    [[nodiscard]] constexpr T &rvec<3, T>::operator[](size_t i)
    {
        assert(i < size);
        return data[i];
    }
    template <Primitive T>
    [[nodiscard]] constexpr T const &rvec<3, T>::operator[](size_t i) const
    {
        assert(i < size);
        return data[i];
    }

    template <Primitive T>
    template <size_t n>
    [[nodiscard]] constexpr rvec<n, T> rvec<3, T>::as_rvec() noexcept requires(n >= 2 && n <= size)
    {
        return rvec<n, T>{*this};
    }

    template <Primitive T>
    template <size_t n, Primitive U>
    [[nodiscard]] constexpr vec<n, std::remove_const_t<U>> rvec<3, T>::as_vec() const noexcept requires(n >= 2 && n <= size)
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
    [[nodiscard]] constexpr rvec<n, const T> rvec<3, T>::as_crvec() const noexcept requires(n >= 2 && n <= size)
    {
        return rvec<n, const T>{*this};
    }
} // namespace engine::core::math
