#pragma once
#include "vec2.hpp"
namespace engine::core::math
{
    template <Primitive T>
    template <Primitive U>
    constexpr vec<2, T>::vec(U value) { x = y = static_cast<T>(value); }
    template <Primitive T>
    template <Primitive A, Primitive B>
    constexpr vec<2, T>::vec(A a, B b)
    {
        x = a;
        y = b;
    }
    template <Primitive T>
    template <typename... U>
    constexpr vec<2, T>::vec(U... data)
    {
        static_assert(get_parameter_pack_size<U...>() == size,
                                    "You have provided wrong amount of data");
        unpack_data(0, data...);
    }
    template <Primitive T>
    constexpr void vec<2, T>::reset() noexcept { x = y = 0; }

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
    constexpr vec<2, T> &vec<2, T>::operator%=(U const value) noexcept
    {
        x %= value;
        y %= value;
        return *this;
    }
    template <Primitive T>
    template <AnyVec U>
    constexpr vec<2, T> &vec<2, T>::operator+=(U const &other) noexcept requires(size == U::size)
    {
        x += other.x;
        y += other.y;
        return *this;
    }
    template <Primitive T>
    template <AnyVec U>
    constexpr vec<2, T> &vec<2, T>::operator-=(U const &other) noexcept requires(size == U::size)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }
    template <Primitive T>
    template <AnyVec U>
    constexpr vec<2, T> &vec<2, T>::operator*=(U const &other) noexcept requires(size == U::size)
    {
        x *= other.x;
        y *= other.y;
        return *this;
    }
    template <Primitive T>
    template <AnyVec U>
    constexpr vec<2, T> &vec<2, T>::operator/=(U const &other) noexcept requires(size == U::size)
    {
        x /= other.x;
        y /= other.y;
        return *this;
    }
    template <Primitive T>
    template <AnyVec U>
    constexpr vec<2, T> &vec<2, T>::operator%=(U const &other) noexcept requires(size == U::size)
    {
        x %= other.x;
        y %= other.y;
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

    template <Primitive T>
    template <Primitive _> // primitives
    constexpr size_t vec<2, T>::get_parameter_pack_size()
    {
        return 1;
    }
    template <Primitive T>
    template <class V> // vectors
    constexpr size_t vec<2, T>::get_parameter_pack_size()
    {
        return V::size;
    }
    template <Primitive T>
    template <typename A, typename B, typename... C>
    constexpr size_t vec<2, T>::get_parameter_pack_size()
    {
        return get_parameter_pack_size<A>() + get_parameter_pack_size<B, C...>();
    }

    template <Primitive T>
    template <Primitive U> // primitives
    constexpr void vec<2, T>::unpack_data(int offset, U u)
    {
        data[offset] = static_cast<T>(u);
    }
    template <Primitive T>
    template <class V> // vectors
    constexpr void vec<2, T>::unpack_data(int offset, V vec)
    {
        for (int i = 0; i < V::size; i++)
        {
            data[offset + i] = static_cast<T>(vec[i]);
        }
    }
    template <Primitive T>
    template <typename A, typename B, typename... C>
    constexpr void vec<2, T>::unpack_data(int offset, A a, B b, C... c)
    {
        unpack_data(offset, a);
        offset += get_parameter_pack_size<A>();
        unpack_data(offset, b, c...);
    }

} // namespace engine::core::math
