#pragma once

#include "vec.hpp"

namespace engine::math
{

    template <Primitive T>
    class type_vec4 final
    {
    public:
        explicit constexpr type_vec4(T value = 0) { x = y = z = w = value; }
        constexpr type_vec4(T x, T y, T z, T w)
        {
            this->x = x;
            this->y = y;
            this->z = z;
            this->w = w;
        }
        [[nodiscard]] constexpr T r() const noexcept { return x; }
        [[nodiscard]] constexpr T g() const noexcept { return y; }
        [[nodiscard]] constexpr T b() const noexcept { return z; }
        [[nodiscard]] constexpr T a() const noexcept { return w; }

        [[nodiscard]] constexpr float length() const noexcept
        {
            return std::sqrt(squared_length());
        }
        [[nodiscard]] constexpr float squared_length() const noexcept
        {
            return x * x + y * y + z * z + w * w;
        }

        [[nodiscard]] constexpr type_vec4<T> unit_vector() const noexcept
        {
            return *this / length();
        }
        constexpr void make_unit_vector() { operator/=(length()); }

        [[nodiscard]] constexpr type_vec4<T> const &operator+() const noexcept
        {
            return *this;
        }
        [[nodiscard]] constexpr type_vec4<T> operator-() const noexcept
        {
            return type_vec4<T>{-x, -y, -z, -w};
        }

        template <Primitive U>
        constexpr type_vec4<T> &operator+=(U const value) noexcept
        {
            x += value;
            y += value;
            z += value;
            w += value;
            return *this;
        }
        template <Primitive U>
        constexpr type_vec4<T> &operator-=(U const value) noexcept
        {
            x -= value;
            y -= value;
            z -= value;
            w -= value;
            return *this;
        }
        template <Primitive U>
        constexpr type_vec4<T> &operator*=(U const value) noexcept
        {
            x *= value;
            y *= value;
            z *= value;
            w *= value;
            return *this;
        }
        template <Primitive U>
        constexpr type_vec4<T> &operator/=(U const value) noexcept
        {
            x /= value;
            y /= value;
            z /= value;
            w /= value;
            return *this;
        }

        template <Primitive U>
        [[nodiscard]] constexpr type_vec4<T> operator+(U const value) const noexcept
        {
            type_vec4<T> return_value{*this};
            return_value += value;
            return return_value;
        }
        template <Primitive U>
        [[nodiscard]] constexpr type_vec4<T> operator-(U const value) const noexcept
        {
            type_vec4<T> return_value{*this};
            return_value -= value;
            return return_value;
        }
        template <Primitive U>
        [[nodiscard]] constexpr type_vec4<T> operator*(U const value) const noexcept
        {
            type_vec4<T> return_value{*this};
            return_value *= value;
            return return_value;
        }
        template <Primitive U>
        [[nodiscard]] constexpr type_vec4<T> operator/(U const value) const noexcept
        {
            type_vec4<T> return_value{*this};
            return_value /= value;
            return return_value;
        }
        template <Primitive U>
        constexpr type_vec4<T> &operator+=(type_vec4<U> const &other) noexcept
        {
            x += other.x;
            y += other.y;
            z += other.z;
            w += other.w;
            return *this;
        }
        template <Primitive U>
        constexpr type_vec4<T> &operator-=(type_vec4<U> const &other) noexcept
        {
            x -= other.x;
            y -= other.y;
            z -= other.z;
            w -= other.w;
            return *this;
        }
        template <Primitive U>
        constexpr type_vec4<T> &operator*=(type_vec4<U> const &other) noexcept
        {
            x *= other.x;
            y *= other.y;
            z *= other.z;
            w *= other.w;
            return *this;
        }
        template <Primitive U>
        constexpr type_vec4<T> &operator/=(type_vec4<U> const &other) noexcept
        {
            x /= other.x;
            y /= other.y;
            z /= other.z;
            w /= other.w;
            return *this;
        }

        template <Primitive U>
        [[nodiscard]] constexpr type_vec4<T>
        operator+(type_vec4<U> const &other) const noexcept
        {
            type_vec4<T> return_value{*this};
            return_value += other;
            return return_value;
        }
        template <Primitive U>
        [[nodiscard]] constexpr type_vec4<T>
        operator-(type_vec4<U> const &other) const noexcept
        {
            type_vec4<T> return_value{*this};
            return_value -= other;
            return return_value;
        }
        template <Primitive U>
        [[nodiscard]] constexpr type_vec4<T>
        operator*(type_vec4<U> const &other) const noexcept
        {
            type_vec4<T> return_value{*this};
            return_value *= other;
            return return_value;
        }
        template <Primitive U>
        [[nodiscard]] constexpr type_vec4<T> operator/(type_vec4<U> const &other) const noexcept
        {
            type_vec4<T> return_value{*this};
            return_value /= other;
            return return_value;
        }

        T x;
        T y;
        T z;
        T w;
    };
    template <Primitive T>
    constexpr std::istream &operator>>(std::istream &is, type_vec4<T> &vec)
    {
        is >> vec.x >> vec.y >> vec.z >> vec.w;
        return is;
    }
    template <Primitive T>
    constexpr std::ostream &operator>>(std::ostream &os, type_vec4<T> &vec)
    {
        os << vec.x << " " << vec.y << " " << vec.z << " " << vec.w;
        return os;
    }

    template <Primitive T, Primitive U>
    constexpr T dot(type_vec4<T> const &left, type_vec4<U> const &right)
    {
        return left.x * right.x + left.y * right.y + left.z * right.z +
               left.w * right.w;
    }
    template <Primitive T, Primitive U>
    constexpr T angle(type_vec4<T> const &left, type_vec4<U> const &right)
    {
        return std::acos(dot(left, right) / left.length() / right.length());
    }
    template <Primitive T, Primitive U>
    [[nodiscard]] constexpr type_vec4<T> operator*(U const value,
                                                   type_vec4<T> const &vec)
    {
        type_vec4<T> return_value{vec};
        return_value *= value;
        return return_value;
    }
    template <Primitive T, Primitive U>
    [[nodiscard]] constexpr type_vec4<T> operator/(U const value,
                                                   type_vec4<T> const &vec)
    {
        type_vec4<T> return_value{vec};
        return_value /= value;
        return return_value;
    }

    using vec4 = type_vec4<float>;
    using dvec4 = type_vec4<double>;
    using ivec4 = type_vec4<int>;
    using bvec4 = type_vec4<bool>;
}; // namespace engine::math