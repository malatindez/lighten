#pragma once

#include "vec.hpp"

namespace engine::math
{

    template <Primitive T>
    struct vec<3, T>
    {
        [[nodiscard]] static constexpr size_t size() { return 3; }
        constexpr vec() = default;
        explicit constexpr vec(T value = 0) { x = y = z = value; }
        constexpr vec(T x, T y, T z)
        {
            this->x = x;
            this->y = y;
            this->z = z;
        }

        [[nodiscard]] constexpr float length() const noexcept
        {
            return std::sqrt(squared_length());
        }
        [[nodiscard]] constexpr float squared_length() const noexcept
        {
            return x * x + y * y + z * z;
        }

        [[nodiscard]] constexpr vec<3, T> unit_vector() const noexcept
        {
            return *this / length();
        }
        constexpr void make_unit_vector() { operator/=(length()); }

        [[nodiscard]] constexpr vec<3, T> const &operator+() const noexcept
        {
            return *this;
        }
        [[nodiscard]] constexpr vec<3, T> operator-() const noexcept
        {
            return vec<3, T>{-x, -y, -z};
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

    using vec3 = vec<3, float>;
    using dvec3 = vec<3, double>;
    using ivec3 = vec<3, int>;
    using bvec3 = vec<3, bool>;
}; // namespace engine::math