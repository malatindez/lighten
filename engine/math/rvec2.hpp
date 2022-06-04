#pragma once

#include "vec.hpp"

namespace engine::core::math
{

    template <Primitive T>
    struct rvec<2, T>
    {
        using type = T;
        static constexpr size_t size = 2;
        explicit constexpr rvec(T a, T b) : x{a}, y{b} {}
        template <AnyVec T>
        explicit constexpr rvec(T &other) requires(T::size >= size) : x{other.x}, y{other.y} {}
        template <AnyVec T>
        explicit constexpr rvec(T const &other) requires(T::size >= size && std::is_const_v<typename T::type>) : x{other.x}, y{other.y} {}
        template <AnyVec U>
        static constexpr rvec<2, T> from_vec(U &other) requires (U::size >= 2) { return rvec<2, T>{other.x, other.y}; }
        template <AnyVec U>
        static constexpr rvec<2, T> from_vec(U const&other) requires (U::size >= 2) { return rvec<2, T>{other.x, other.y}; }
        constexpr void reset() noexcept;

        template <typename U>
        constexpr rvec<size, T> &operator=(rvec<size, U> const &b)
        {
            for (int i = 0; i < size; i++)
            {
                data[i] = b.data[i];
            }
            return *this;
        }
        template <typename U>
        constexpr rvec<size, T> &operator=(vec<size, U> const &b)
        {
            for (int i = 0; i < size; i++)
            {
                data[i] = b.data[i];
            }
            return *this;
        }

        [[nodiscard]] constexpr rvec<2, T> const &operator+() const noexcept;

        [[nodiscard]] constexpr vec<2, T> operator-() const noexcept;

        template <Primitive U>
        constexpr rvec<2, T> &operator+=(U const value) noexcept;
        template <Primitive U>
        constexpr rvec<2, T> &operator-=(U const value) noexcept;
        template <Primitive U>
        constexpr rvec<2, T> &operator*=(U const value) noexcept;
        template <Primitive U>
        constexpr rvec<2, T> &operator/=(U const value) noexcept;
        template <Primitive U>
        constexpr rvec<2, T> &operator%=(U const value) noexcept;
        template <AnyVec U>
        constexpr rvec<2, T> &operator+=(U const &other) noexcept requires (size == U::size);
        template <AnyVec U>
        constexpr rvec<2, T> &operator-=(U const &other) noexcept requires (size == U::size);
        template <AnyVec U>
        constexpr rvec<2, T> &operator*=(U const &other) noexcept requires (size == U::size);
        template <AnyVec U>
        constexpr rvec<2, T> &operator/=(U const &other) noexcept requires (size == U::size);
        template <AnyVec U>
        constexpr rvec<2, T> &operator%=(U const &other) noexcept requires (size == U::size);


        [[nodiscard]] constexpr T &operator[](size_t i);
        [[nodiscard]] constexpr T const &operator[](size_t i) const;

        constexpr explicit operator rvec<2, const T>() const noexcept { return rvec<2, const T>{x, y}; }
        union
        {
            struct
            {
                union { _detail::primitive_reference_wrapper<T> x, r, s, u; };
                union {  _detail::primitive_reference_wrapper<T> y, g, t, v; };
            };
            std::array< _detail::primitive_reference_wrapper<T>, 2> data;
        };
        static_assert(sizeof(data) == 2 * sizeof( _detail::primitive_reference_wrapper<T>));
    };

}; // namespace engine::core::math

#include "rvec2.inl"