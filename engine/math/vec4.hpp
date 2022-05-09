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

        void reset()
        {
            x = y = z = w = 0;
        }

        [[nodiscard]] constexpr vec<4, T> const &operator+() const noexcept
        {
            return *this;
        }
        [[nodiscard]] constexpr vec<4, T> operator-() const noexcept
        {
            return vec<4, T>(-x, -y, -z, -w);
        }

        template <Primitive U>
        constexpr vec<4, T> &operator+=(U const value) noexcept
        {
            x += value;
            y += value;
            z += value;
            w += value;
            return *this;
        }
        template <Primitive U>
        constexpr vec<4, T> &operator-=(U const value) noexcept
        {
            x -= value;
            y -= value;
            z -= value;
            w -= value;
            return *this;
        }
        template <Primitive U>
        constexpr vec<4, T> &operator*=(U const value) noexcept
        {
            x *= value;
            y *= value;
            z *= value;
            w *= value;
            return *this;
        }
        template <Primitive U>
        constexpr vec<4, T> &operator/=(U const value) noexcept
        {
            x /= value;
            y /= value;
            z /= value;
            w /= value;
            return *this;
        }
        template <Primitive U>
        constexpr vec<4, T> &operator+=(vec<4, U> const &other) noexcept
        {
            x += other.x;
            y += other.y;
            z += other.z;
            w += other.w;
            return *this;
        }
        template <Primitive U>
        constexpr vec<4, T> &operator-=(vec<4, U> const &other) noexcept
        {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            w -= other.w;
            return *this;
        }
        template <Primitive U>
        constexpr vec<4, T> &operator*=(vec<4, U> const &other) noexcept
        {
            x *= other.x;
            y *= other.y;
            z *= other.z;
            w *= other.w;
            return *this;
        }
        template <Primitive U>
        constexpr vec<4, T> &operator/=(vec<4, U> const &other) noexcept
        {
            x /= other.x;
            y /= other.y;
            z /= other.z;
            w /= other.w;
            return *this;
        }

        [[nodiscard]] constexpr T &operator[](size_t i)
        {
            assert(i < size);
            return data[i];
        }
        [[nodiscard]] constexpr T const &operator[](size_t i) const
        {
            assert(i < size);
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
                union
                {
                    T z, b, p;
                };
                union
                {
                    T w, a, q;
                };
            };
            std::array<T, size> data;
        };
        static_assert(sizeof(data) == size * sizeof(T));
    };

}; // namespace engine::math
