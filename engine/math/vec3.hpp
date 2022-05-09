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

        void reset()
        {
            x = y = z = 0;
        }

        [[nodiscard]] constexpr vec<3, T> const &operator+() const noexcept
        {
            return *this;
        }
        [[nodiscard]] constexpr vec<3, T> operator-() const noexcept
        {
            return vec<3, T>(-x, -y, -z);
        }

        template <Primitive U>
        constexpr vec<3, T> &operator+=(U const value) noexcept
        {
            x += value;
            y += value;
            z += value;
            return *this;
        }
        template <Primitive U>
        constexpr vec<3, T> &operator-=(U const value) noexcept
        {
            x -= value;
            y -= value;
            z -= value;
            return *this;
        }
        template <Primitive U>
        constexpr vec<3, T> &operator*=(U const value) noexcept
        {
            x *= value;
            y *= value;
            z *= value;
            return *this;
        }
        template <Primitive U>
        constexpr vec<3, T> &operator/=(U const value) noexcept
        {
            x /= value;
            y /= value;
            z /= value;
            return *this;
        }
        template <Primitive U>
        constexpr vec<3, T> &operator+=(vec<3, U> const &other) noexcept
        {
            x += other.x;
            y += other.y;
            z += other.z;
            return *this;
        }
        template <Primitive U>
        constexpr vec<3, T> &operator-=(vec<3, U> const &other) noexcept
        {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            return *this;
        }
        template <Primitive U>
        constexpr vec<3, T> &operator*=(vec<3, U> const &other) noexcept
        {
            x *= other.x;
            y *= other.y;
            z *= other.z;
            return *this;
        }
        template <Primitive U>
        constexpr vec<3, T> &operator/=(vec<3, U> const &other) noexcept
        {
            x /= other.x;
            y /= other.y;
            z /= other.z;
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
            };
            std::array<T, 3> data;
        };
        static_assert(sizeof(data) == 3 * sizeof(T));
    };

    template <Primitive T, Primitive U>
    constexpr vec<3, T> cross(vec<3, T> const &left,
                              vec<3, U> const &right)
    {
        return vec<3, T>{left.y * right.z - left.z * right.y,
                         left.x * right.z - left.z * right.x,
                         left.x * right.y - left.y * right.x};
    }

}; // namespace engine::math