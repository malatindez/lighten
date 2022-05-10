#pragma once

#include "vec.hpp"
#include <cstddef>

namespace engine::math
{

    template <Primitive T>
    struct vec<4, T>
    {
        static constexpr size_t size = 4;
        constexpr vec() = default;
        explicit constexpr vec(T value) { x = y = z = w = value; }
        constexpr vec(T x, T y, T z, T w)
        {
            this->x = x;
            this->y = y;
            this->z = z;
            this->w = w;
        }

        constexpr void reset() noexcept;

        [[nodiscard]] constexpr vec<4, T> const &operator+() const noexcept;
        [[nodiscard]] constexpr vec<4, T> operator-() const noexcept;

        template <Primitive U>
        constexpr vec<4, T> &operator+=(U const value) noexcept;
        template <Primitive U>
        constexpr vec<4, T> &operator-=(U const value) noexcept;
        template <Primitive U>
        constexpr vec<4, T> &operator*=(U const value) noexcept;
        template <Primitive U>
        constexpr vec<4, T> &operator/=(U const value) noexcept;
        template <Primitive U>
        constexpr vec<4, T> &operator+=(vec<4, U> const &other) noexcept;
        template <Primitive U>
        constexpr vec<4, T> &operator-=(vec<4, U> const &other) noexcept;
        template <Primitive U>
        constexpr vec<4, T> &operator*=(vec<4, U> const &other) noexcept;
        template <Primitive U>
        constexpr vec<4, T> &operator/=(vec<4, U> const &other) noexcept;

        [[nodiscard]] constexpr T &operator[](size_t i);
        [[nodiscard]] constexpr T const &operator[](size_t i) const;
        union
        {
            struct
            {
                union { T x, r, s; };
                union { T y, g, t; };
                union { T z, b, p; };
                union { T w, a, q; };
            };
            std::array<T, size> data;
        };
        static_assert(sizeof(data) == size * sizeof(T));
    };

}; // namespace engine::math

#include "vec4.inl"