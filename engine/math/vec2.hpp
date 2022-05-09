#pragma once

#include "vec.hpp"

namespace engine::math
{

    template <Primitive T>
    struct vec<2, T>
    {
        [[nodiscard]] static constexpr size_t size() { return 2; }
        constexpr vec() = default;
        explicit constexpr vec(T value = 0) { x = y = value; }
        constexpr vec(T x, T y)
        {
            this->x = x;
            this->y = y;
        }

        [[nodiscard]] constexpr float length() const noexcept
        {
            return std::sqrt(squared_length());
        }
        [[nodiscard]] constexpr float squared_length() const noexcept
        {
            return x * x + y * y;
        }

        [[nodiscard]] constexpr vec<2, T> unit_vector() const noexcept
        {
            return *this / length();
        }
        constexpr void make_unit_vector() { operator/=(length()); }

        [[nodiscard]] constexpr vec<2, T> const &operator+() const noexcept
        {
            return *this;
        }

        [[nodiscard]] constexpr vec<2, T> operator-() const noexcept
        {
            return vec<2, T>(-static_cast<T>(x), -static_cast<T>(y));
        }

        template <Primitive U>
        constexpr vec<2, T> &operator+=(U const value) noexcept
        {
            x += value;
            y += value;
            return *this;
        }
        template <Primitive U>
        constexpr vec<2, T> &operator-=(U const value) noexcept
        {
            x -= value;
            y -= value;
            return *this;
        }
        template <Primitive U>
        constexpr vec<2, T> &operator*=(U const value) noexcept
        {
            x *= value;
            y *= value;
            return *this;
        }
        template <Primitive U>
        constexpr vec<2, T> &operator/=(U const value) noexcept
        {
            x /= value;
            y /= value;
            return *this;
        }
        template <Primitive U>
        constexpr vec<2, T> &operator+=(vec<2, U> const &other) noexcept
        {
            x += other.x;
            y += other.y;
            return *this;
        }
        template <Primitive U>
        constexpr vec<2, T> &operator-=(vec<2, U> const &other) noexcept
        {
            x -= other.x;
            y -= other.y;
            return *this;
        }
        template <Primitive U>
        constexpr vec<2, T> &operator*=(vec<2, U> const &other) noexcept
        {
            x *= other.x;
            y *= other.y;
            return *this;
        }
        template <Primitive U>
        constexpr vec<2, T> &operator/=(vec<2, U> const &other) noexcept
        {
            x /= other.x;
            y /= other.y;
            return *this;
        }
        [[nodiscard]] constexpr T &operator[](size_t i)
        {
            assert(i < size());
            return data[i];
        }
        [[nodiscard]] constexpr T const &operator[](size_t i) const
        {
            assert(i < size());
            return data[i];
        }
        union
        {
            struct
            {
                union
                {
                    T x, r, s;
                };
                union
                {
                    T y, g, t;
                };
            };
            std::array<T, 2> data;
        };
        static_assert(sizeof(data) == 2 * sizeof(T));
    };

    using vec2 = vec<2, float>;
    using dvec2 = vec<2, double>;
    using ivec2 = vec<2, int>;
    using bvec2 = vec<2, bool>;

}; // namespace engine::math
