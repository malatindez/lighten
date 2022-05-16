#pragma once
#include "quaternion.hpp"

namespace engine::math
{

    template <Primitive T>
    [[nodiscard]] constexpr qua<T> const &qua<T>::operator+() const noexcept
    {
        return *this;
    }
    template <Primitive T>
    [[nodiscard]] constexpr qua<T> qua<T>::operator-() const noexcept
    {
        return qua<T>(-x, -y, -z, -w);
    }

    template <Primitive T>
    template <Primitive U>
    constexpr qua<T> &qua<T>::operator+=(U const value) noexcept
    {
        x += value;
        y += value;
        z += value;
        w += value;
        return *this;
    }
    template <Primitive T>
    template <Primitive U>
    constexpr qua<T> &qua<T>::operator-=(U const value) noexcept
    {
        x -= value;
        y -= value;
        z -= value;
        w -= value;
        return *this;
    }
    template <Primitive T>
    template <Primitive U>
    constexpr qua<T> &qua<T>::operator*=(U const value) noexcept
    {
        x *= value;
        y *= value;
        z *= value;
        w *= value;
        return *this;
    }
    template <Primitive T>
    template <Primitive U>
    constexpr qua<T> &qua<T>::operator/=(U const value) noexcept
    {
        x /= value;
        y /= value;
        z /= value;
        w /= value;
        return *this;
    }
    template <Primitive T>
    template <Primitive U>
    constexpr qua<T> &qua<T>::operator+=(qua<U> const &other) noexcept
    {
        x += other.x;
        y += other.y;
        z += other.z;
        w += other.w;
        return *this;
    }
    template <Primitive T>
    template <Primitive U>
    constexpr qua<T> &qua<T>::operator-=(qua<U> const &other) noexcept
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        w -= other.w;
        return *this;
    }
    template <Primitive T>
    template <Primitive U>
    constexpr qua<T> &qua<T>::operator*=(qua<U> const &other) noexcept
    {
        qua<T> const p(*this);
        qua<T> const q(other);

        this->w = p.w * q.w - p.x * q.x - p.y * q.y - p.z * q.z;
        this->x = p.w * q.x + p.x * q.w + p.y * q.z - p.z * q.y;
        this->y = p.w * q.y + p.y * q.w + p.z * q.x - p.x * q.z;
        this->z = p.w * q.z + p.z * q.w + p.x * q.y - p.y * q.x;
        return *this;
    }
    template <Primitive T>
    template <Primitive U>
    constexpr qua<T> &qua<T>::operator/=(qua<U> const &other) noexcept
    {
        x /= other.x;
        y /= other.y;
        z /= other.z;
        w /= other.w;
        return *this;
    }

    template <Primitive T>
    [[nodiscard]] constexpr T &qua<T>::operator[](size_t i)
    {
        assert(i < 4);
        return data[i];
    }
    template <Primitive T>
    [[nodiscard]] constexpr T const &qua<T>::operator[](size_t i) const
    {
        assert(i < 4);
        return data[i];
    }
    template <Primitive T>
    constexpr std::istream &operator>>(std::istream &is, qua<T> &vec)
    {
        for (int i = 0; i < 4; i++)
        {
            is >> vec[i];
        }
        return is;
    }
    template <Primitive T>
    constexpr std::ostream &operator<<(std::ostream &os, qua<T> &vec)
    {
        for (int i = 0; i < 4; i++)
        {
            os << vec[i] << " ";
        }
        return os;
    }

    template <Primitive T, Primitive U>
    [[nodiscard]] constexpr qua<T> operator*(U const value, qua<T> const &vector)
    {
        return qua<T>(vector) *= value;
    }
    template <Primitive T, Primitive U>
    [[nodiscard]] constexpr qua<T> operator+(qua<T> const &vector, U const value) noexcept
    {
        return qua<T>(vector) += value;
    }
    template <Primitive T, Primitive U>
    [[nodiscard]] constexpr qua<T> operator-(qua<T> const &vector, U const value) noexcept
    {
        return qua<T>(vector) -= value;
    }
    template <Primitive T, Primitive U>
    [[nodiscard]] constexpr qua<T> operator*(qua<T> const &vector, U const value) noexcept
    {
        return qua<T>(vector) *= value;
    }
    template <Primitive T, Primitive U>
    [[nodiscard]] constexpr qua<T> operator/(qua<T> const &vector, U const value) noexcept
    {
        return qua<T>(vector) /= value;
    }
    template <Primitive T, Primitive U>
    [[nodiscard]] constexpr qua<T> operator+(qua<T> const &left, qua<U> const &right) noexcept
    {
        return qua<T>(left) += right;
    }
    template <Primitive T, Primitive U>
    [[nodiscard]] constexpr qua<T> operator-(qua<T> const &left, qua<U> const &right) noexcept
    {
        return qua<T>(left) -= right;
    }
    template <Primitive T, Primitive U>
    [[nodiscard]] constexpr qua<T> operator*(qua<T> const &left, qua<U> const &right) noexcept
    {
        return qua<T>(left) *= right;
    }
    template <Primitive T, Primitive U>
    [[nodiscard]] constexpr qua<T> operator/(qua<T> const &left, qua<U> const &right) noexcept
    {
        return qua<T>(left) /= right;
    }

    template <Primitive T, Primitive U>
    constexpr T dot(qua<T> const &left, qua<U> const &right)
    {
        T return_value = 0;
        for (size_t i = 0; i < 4; i++)
        {
            return_value += left[i] * static_cast<T>(right[i]);
        }
        return return_value;
    }

    template <Primitive T>
    qua<T> conjugate(qua<T> const &q)
    {
        return qua<T>(q.w, -q.x, -q.y, -q.z);
    }

    template <Primitive T>
    qua<T> inverse(qua<T> const &q)
    {
        return conjugate(q) / dot(q, q);
    }

    template <Primitive T, Primitive U>
    constexpr vec<3, T> cross(qua<T> const &left, vec<3, U> const &right)
    {
        return left * right;
    }
    template <Primitive T, Primitive U>
    constexpr vec<3, T> cross(vec<3, U> const &left, qua<T> const &right)
    {
        return inverse(left) * right;
    }
}