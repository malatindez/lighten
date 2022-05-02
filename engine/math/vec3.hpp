#pragma once

#include "vec.hpp"

namespace engine::math
{

    template <Primitive T>
    class type_vec3 final
    {
    public:
        explicit constexpr type_vec3(T value = 0) { x = y = z = value; }
        constexpr type_vec3(T x, T y, T z)
        {
            this->x = x;
            this->y = y;
            this->z = z;
        }
        [[nodiscard]] constexpr T r() const noexcept { return x; }
        [[nodiscard]] constexpr T g() const noexcept { return y; }
        [[nodiscard]] constexpr T b() const noexcept { return z; }

        [[nodiscard]] constexpr float length() const noexcept
        {
            return std::sqrt(squared_length());
        }
        [[nodiscard]] constexpr float squared_length() const noexcept
        {
            return x * x + y * y + z * z;
        }

        [[nodiscard]] constexpr type_vec3<T> unit_vector() const noexcept
        {
            return *this / length();
        }
        constexpr void make_unit_vector() { operator/=(length()); }

        [[nodiscard]] constexpr type_vec3<T> const &operator+() const noexcept
        {
            return *this;
        }
        [[nodiscard]] constexpr type_vec3<T> operator-() const noexcept
        {
            return type_vec3<T>{-x, -y, -z};
        }

        template <Primitive U>
        constexpr type_vec3<T> &operator+=(U const value) noexcept
        {
            x += value;
            y += value;
            z += value;
            return *this;
        }
        template <Primitive U>
        constexpr type_vec3<T> &operator-=(U const value) noexcept
        {
            x -= value;
            y -= value;
            z -= value;
            return *this;
        }
        template <Primitive U>
        constexpr type_vec3<T> &operator*=(U const value) noexcept
        {
            x *= value;
            y *= value;
            z *= value;
            return *this;
        }
        template <Primitive U>
        constexpr type_vec3<T> &operator/=(U const value) noexcept
        {
            x /= value;
            y /= value;
            z /= value;
            return *this;
        }

        template <Primitive U>
        [[nodiscard]] constexpr type_vec3<T> operator+(U const value) const noexcept
        {
            type_vec3<T> return_value{*this};
            return_value += value;
            return return_value;
        }
        template <Primitive U>
        [[nodiscard]] constexpr type_vec3<T> operator-(U const value) const noexcept
        {
            type_vec3<T> return_value{*this};
            return_value -= value;
            return return_value;
        }
        template <Primitive U>
        [[nodiscard]] constexpr type_vec3<T> operator*(U const value) const noexcept
        {
            type_vec3<T> return_value{*this};
            return_value *= value;
            return return_value;
        }
        template <Primitive U>
        [[nodiscard]] constexpr type_vec3<T> operator/(U const value) const noexcept
        {
            type_vec3<T> return_value{*this};
            return_value /= value;
            return return_value;
        }
        template <Primitive U>
        constexpr type_vec3<T> &operator+=(type_vec3<U> const &other) noexcept
        {
            x += other.x;
            y += other.y;
            z += other.z;
            return *this;
        }
        template <Primitive U>
        constexpr type_vec3<T> &operator-=(type_vec3<U> const &other) noexcept
        {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            return *this;
        }
        template <Primitive U>
        constexpr type_vec3<T> &operator*=(type_vec3<U> const &other) noexcept
        {
            x *= other.x;
            y *= other.y;
            z *= other.z;
            return *this;
        }
        template <Primitive U>
        constexpr type_vec3<T> &operator/=(type_vec3<U> const &other) noexcept
        {
            x /= other.x;
            y /= other.y;
            z /= other.z;
            return *this;
        }

        template <Primitive U>
        [[nodiscard]] constexpr type_vec3<T>
        operator+(type_vec3<U> const &other) const noexcept
        {
            type_vec3<T> return_value{*this};
            return_value += other;
            return return_value;
        }
        template <Primitive U>
        [[nodiscard]] constexpr type_vec3<T>
        operator-(type_vec3<U> const &other) const noexcept
        {
            type_vec3<T> return_value{*this};
            return_value -= other;
            return return_value;
        }
        template <Primitive U>
        [[nodiscard]] constexpr type_vec3<T>
        operator*(type_vec3<U> const &other) const noexcept
        {
            type_vec3<T> return_value{*this};
            return_value *= other;
            return return_value;
        }
        template <Primitive U>
        [[nodiscard]] constexpr type_vec3<T>
        operator/(type_vec3<U> const &other) const noexcept
        {
            type_vec3<T> return_value{*this};
            return_value /= other;
            return return_value;
        }

        T x;
        T y;
        T z;
    };
    template <Primitive T>
    constexpr std::istream &operator>>(std::istream &is, type_vec3<T> &vec)
    {
        is >> vec.x >> vec.y >> vec.z;
        return is;
    }
    template <Primitive T>
    constexpr std::ostream &operator>>(std::ostream &os, type_vec3<T> &vec)
    {
        os << vec.x << " " << vec.y << " " << vec.z;
        return os;
    }

    template <Primitive T, Primitive U>
    constexpr T dot(type_vec3<T> const &left, type_vec3<U> const &right)
    {
        return left.x * right.x + left.y * right.y + left.z * right.z;
    }
    template <Primitive T, Primitive U>
    constexpr T angle(type_vec3<T> const &left, type_vec3<U> const &right)
    {
        return std::acos(dot(left, right) / left.length() / right.length());
    }

    template <Primitive T, Primitive U>
    constexpr type_vec3<T> cross(type_vec3<T> const &left,
                                 type_vec3<U> const &right)
    {
        return type_vec3<T>{left.y * right.z - left.z * right.y,
                            left.x * right.z - left.z * right.x,
                            left.x * right.y - left.y * right.x};
    }

    template <Primitive T, Primitive U>
    [[nodiscard]] constexpr type_vec3<T> operator*(U const value,
                                                   type_vec3<T> const &vec)
    {
        type_vec3<T> return_value{vec};
        return_value *= value;
        return return_value;
    }
    template <Primitive T, Primitive U>
    [[nodiscard]] constexpr type_vec3<T> operator/(U const value,
                                                   type_vec3<T> const &vec)
    {
        type_vec3<T> return_value{vec};
        return_value /= value;
        return return_value;
    }

    using vec3 = type_vec3<float>;
    using dvec3 = type_vec3<double>;
    using ivec3 = type_vec3<int>;
    using bvec3 = type_vec3<bool>;
}; // namespace engine::math