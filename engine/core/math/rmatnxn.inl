#pragma once
#include "rmatnxn.hpp"

namespace engine::core::math
{
    template <size_t a, size_t b, Primitive T>
    template <size_t c, size_t d, Primitive P>
    constexpr rmat<a, b, T>::rmat(mat<c, d, P> &p) requires(c >= a && d >= b)
    {
        for (size_t i = 0; i < a; i++)
        {
            for (size_t j = 0; j < b; j++)
            {
                arr[i * b + j].set_ptr(p.data[i][j]);
            }
        }
    }
    template <size_t a, size_t b, Primitive T>
    template <size_t c, size_t d, Primitive P>
    constexpr rmat<a, b, T>::rmat(rmat<c, d, P> &p) requires(c >= a && d >= b)
    {
        for (size_t i = 0; i < a; i++)
        {
            for (size_t j = 0; j < b; j++)
            {
                arr[i * b + j].set_ptr(p.arr[i * b + j]);
            }
        }
    }

    template <size_t a, size_t b, Primitive T>
    constexpr void rmat<a, b, T>::reset() noexcept
    {
        for (size_t i = 0; i < size.x; i++)
        {
            data[i].reset();
        }
        for (size_t i = 0; i < a && i < b; i++)
        {
            data[i][i] = 1;
        }
    }

    template <size_t a, size_t b, Primitive T>
    [[nodiscard]] constexpr _detail::rvec<b, T> &rmat<a, b, T>::operator[](size_t i)
    {
        assert(i < size.x);
        return data[i];
    }
    template <size_t a, size_t b, Primitive T>
    [[nodiscard]] constexpr _detail::rvec<b, T> const &rmat<a, b, T>::operator[](size_t i) const
    {
        assert(i < size.x);
        return data[i];
    }

    template <size_t a, size_t b, Primitive T>
    [[nodiscard]] constexpr rmat<a, b, T> const &rmat<a, b, T>::operator+() const noexcept
    {
        return *this;
    }

    template <size_t a, size_t b, Primitive T>
    [[nodiscard]] constexpr mat<a, b, T> rmat<a, b, T>::operator-() const noexcept
    {
        mat<a, b, T> return_value {};
        for (size_t i = 0; i < size.x; i++)
        {
            return_value.data[i] = -data[i];
        }
        return return_value;
    }

    template <size_t a, size_t b, Primitive T>
    template <Primitive U>
    constexpr rmat<a, b, T> &rmat<a, b, T>::operator+=(rmat<a, b, U> const &other)
    {
        for (size_t i = 0; i < size.x; i++)
        {
            data[i] += other.data[i];
        }
        return *this;
    }
    template <size_t a, size_t b, Primitive T>
    template <Primitive U>
    constexpr rmat<a, b, T> &rmat<a, b, T>::operator-=(rmat<a, b, U> const &other)
    {
        for (size_t i = 0; i < size.x; i++)
        {
            data[i] -= other.data[i];
        }
        return *this;
    }

    template <size_t a, size_t b, Primitive T>
    template <size_t c, Primitive U>
    constexpr rmat<a, c, T> &rmat<a, b, T>::operator*=(rmat<b, c, U> const &other)
    {
        return (*this = *this * other);
    }
    template <size_t a, size_t b, Primitive T>
    template <Primitive U>
    constexpr rmat<a, b, T> &rmat<a, b, T>::operator+=(mat<a, b, U> const &other)
    {
        for (size_t i = 0; i < size.x; i++)
        {
            data[i] += other.data[i];
        }
        return *this;
    }
    template <size_t a, size_t b, Primitive T>
    template <Primitive U>
    constexpr rmat<a, b, T> &rmat<a, b, T>::operator-=(mat<a, b, U> const &other)
    {
        for (size_t i = 0; i < size.x; i++)
        {
            data[i] -= other.data[i];
        }
        return *this;
    }

    template <size_t a, size_t b, Primitive T>
    template <size_t c, Primitive U>
    constexpr rmat<a, c, T> &rmat<a, b, T>::operator*=(mat<b, c, U> const &other)
    {
        return (*this = *this * other);
    }

    template <size_t a, size_t b, Primitive T>
    template <Primitive U>
    constexpr rmat<a, b, T> &rmat<a, b, T>::operator+=(U const value)
    {
        for (size_t i = 0; i < size.x; i++)
        {
            data[i] += value;
        }
        return *this;
    }
    template <size_t a, size_t b, Primitive T>
    template <Primitive U>
    constexpr rmat<a, b, T> &rmat<a, b, T>::operator-=(U const value)
    {
        for (size_t i = 0; i < size.x; i++)
        {
            data[i] -= value;
        }
        return *this;
    }
    template <size_t a, size_t b, Primitive T>
    template <Primitive U>
    constexpr rmat<a, b, T> &rmat<a, b, T>::operator*=(U const value)
    {
        for (size_t i = 0; i < size.x; i++)
        {
            data[i] *= value;
        }
        return *this;
    }
} // namespace engine::core::math