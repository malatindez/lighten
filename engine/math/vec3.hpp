#pragma once

#include "vec.hpp"

namespace engine::math
{

    template <Primitive T>
    struct vec<3, T>
    {
        static constexpr size_t size = 3;
        constexpr vec() = default;
        explicit constexpr vec(T value) { x = y = z = value; }
        constexpr vec(T x, T y, T z)
        {
            this->x = x;
            this->y = y;
            this->z = z;
        }

        constexpr void reset() noexcept;

        [[nodiscard]] constexpr vec<3, T> const &operator+() const noexcept;
        [[nodiscard]] constexpr vec<3, T> operator-() const noexcept;

        template <Primitive U>
        constexpr vec<3, T> &operator+=(U const value) noexcept;
        template <Primitive U>
        constexpr vec<3, T> &operator-=(U const value) noexcept;
        template <Primitive U>
        constexpr vec<3, T> &operator*=(U const value) noexcept;
        template <Primitive U>
        constexpr vec<3, T> &operator/=(U const value) noexcept;
        template <Primitive U>
        constexpr vec<3, T> &operator+=(vec<3, U> const &other) noexcept;
        template <Primitive U>
        constexpr vec<3, T> &operator-=(vec<3, U> const &other) noexcept;
        template <Primitive U>
        constexpr vec<3, T> &operator*=(vec<3, U> const &other) noexcept;
        template <Primitive U>
        constexpr vec<3, T> &operator/=(vec<3, U> const &other) noexcept;

        [[nodiscard]] constexpr T &operator[](size_t i);
        [[nodiscard]] constexpr T const &operator[](size_t i) const;
        union
        {
            struct
            {
                union { T x, r, s; };
                union
                {
                    T y, g, t;
                };
                union
                {
                    T z, b, p;
                };
            };
            std::array<T, 3> data;
        };
        static_assert(sizeof(data) == 3 * sizeof(T));
    };

    template <Primitive T, Primitive U>
    constexpr vec<3, T> cross(vec<3, T> const &left, vec<3, U> const &right);

}; // namespace engine::math
#include "vec3.inl"