#pragma once

#include "vec.hpp"
#include <cstddef>

namespace engine::math
{

    template <Primitive T>
    struct vec<4, T>
    {
        [[nodiscard]] static constexpr size_t size() { return 4; }
        constexpr vec() = default;
        explicit constexpr vec(T value = 0) { x = y = z = w = value; }
        constexpr vec(T x, T y, T z, T w)
        {
            this->x = x;
            this->y = y;
            this->z = z;
            this->w = w;
        }

        [[nodiscard]] constexpr float length() const noexcept
        {
            return std::sqrt(squared_length());
        }
        [[nodiscard]] constexpr float squared_length() const noexcept
        {
            return x * x + y * y + z * z + w * w;
        }

        [[nodiscard]] constexpr vec<4, T> unit_vector() const noexcept
        {
            return *this / length();
        }
        constexpr void make_unit_vector() { operator/=(length()); }

        [[nodiscard]] constexpr vec<4, T> const &operator+() const noexcept
        {
            return *this;
        }
        [[nodiscard]] constexpr vec<4, T> operator-() const noexcept
        {
            return vec<4, T>{-x, -y, -z, -w};
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
                union
                {
                    T w, a, q;
                };
            };
            std::array<T, size()> data;
        };
        static_assert(sizeof(data) == size() * sizeof(T));
    };

    using vec4 = vec<4, float>;
    using dvec4 = vec<4, double>;
    using ivec4 = vec<4, int>;
    using bvec4 = vec<4, bool>;
}; // namespace engine::math
