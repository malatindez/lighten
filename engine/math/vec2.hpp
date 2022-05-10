#pragma once

#include "vec.hpp"

namespace engine::math
{

    template <Primitive T>
    struct vec<2, T>
    {
        static constexpr size_t size = 2;
        constexpr vec() = default;
        explicit constexpr vec(T value) { x = y = value; }
        constexpr vec(T x, T y)
        {
            this->x = x;
            this->y = y;
        }

        constexpr void reset() noexcept;

        [[nodiscard]] constexpr vec<2, T> const &operator+() const noexcept;

        [[nodiscard]] constexpr vec<2, T> operator-() const noexcept;

        template <Primitive U>
        constexpr vec<2, T> &operator+=(U const value) noexcept;
        template <Primitive U>
        constexpr vec<2, T> &operator-=(U const value) noexcept;
        template <Primitive U>
        constexpr vec<2, T> &operator*=(U const value) noexcept;
        template <Primitive U>
        constexpr vec<2, T> &operator/=(U const value) noexcept;
        template <Primitive U>
        constexpr vec<2, T> &operator+=(vec<2, U> const &other) noexcept;
        template <Primitive U>
        constexpr vec<2, T> &operator-=(vec<2, U> const &other) noexcept;
        template <Primitive U>
        constexpr vec<2, T> &operator*=(vec<2, U> const &other) noexcept;
        template <Primitive U>
        constexpr vec<2, T> &operator/=(vec<2, U> const &other) noexcept;
        [[nodiscard]] constexpr T &operator[](size_t i);
        [[nodiscard]] constexpr T const &operator[](size_t i) const;
        union
        {
            struct
            {
                union { T x, r, s; };
                union { T y, g, t; };
            };
            std::array<T, 2> data;
        };
        static_assert(sizeof(data) == 2 * sizeof(T));
    };

}; // namespace engine::math

#include "vec2.inl"