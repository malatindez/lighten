#pragma once
#include "quaternion.hpp"

namespace engine::math
{

    template <Primitive T>
    constexpr qua<T>::qua(T w, T x, T y, T z)
    {
        this->w = w;
        this->x = x;
        this->y = y;
        this->z = z;
    }
    template <Primitive T>
    constexpr void qua<T>::reset()
    {
        x = y = z = T(0);
        w = T(1);
    }

    template <Primitive T>
    constexpr qua<T>::qua(T radians, vec<3, T> axis)
    {
        if (math::length(axis) != 0)
        {
            axis = normalize(axis);
        }
        w = math::cos(radians / 2);
        float const s = math::sin(radians / 2);
        x = axis.x * s;
        y = axis.y * s;
        z = axis.z * s;
    }

    template <Primitive T>
    [[nodiscard]] constexpr mat<3, 3, T> qua<T>::as_mat3() const noexcept
    {
        mat<3, 3, T> return_value(T(1));
        T qxx(x * x);
        T qyy(y * y);
        T qzz(z * z);
        T qxz(x * z);
        T qxy(x * y);
        T qyz(y * z);
        T qwx(w * x);
        T qwy(w * y);
        T qwz(w * z);

        return_value[0][0] = T(1) - T(2) * (qyy + qzz);
        return_value[0][1] = T(2) * (qxy - qwz);
        return_value[0][2] = T(2) * (qxz + qwy);

        return_value[1][0] = T(2) * (qxy + qwz);
        return_value[1][1] = T(1) - T(2) * (qxx + qzz);
        return_value[1][2] = T(2) * (qyz - qwx);

        return_value[2][0] = T(2) * (qxz - qwy);
        return_value[2][1] = T(2) * (qyz + qwx);
        return_value[2][2] = T(1) - T(2) * (qxx + qyy);
        return return_value;
    }

    template <Primitive T>
    [[nodiscard]] constexpr mat<4, 4, T> qua<T>::as_mat4() const noexcept
    {
        return mat<4, 4, T>(as_mat3());
    }
    template <Primitive T>
    [[nodiscard]] constexpr vec<3, T> qua<T>::axis() const noexcept
    {
        return vec<3, T>{x, y, z};
    }
    template <Primitive T>
    [[nodiscard]] constexpr T qua<T>::radians() const noexcept
    {
        return w;
    }

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
    [[nodiscard]] constexpr qua<T> operator+(qua<T> const &vector,
                                             U const value) noexcept
    {
        return qua<T>(vector) += value;
    }
    template <Primitive T, Primitive U>
    [[nodiscard]] constexpr qua<T> operator-(qua<T> const &vector,
                                             U const value) noexcept
    {
        return qua<T>(vector) -= value;
    }
    template <Primitive T, Primitive U>
    [[nodiscard]] constexpr qua<T> operator*(qua<T> const &vector,
                                             U const value) noexcept
    {
        return qua<T>(vector) *= value;
    }
    template <Primitive T, Primitive U>
    [[nodiscard]] constexpr qua<T> operator/(qua<T> const &vector,
                                             U const value) noexcept
    {
        return qua<T>(vector) /= value;
    }
    template <Primitive T, Primitive U>
    [[nodiscard]] constexpr qua<T> operator+(qua<T> const &left,
                                             qua<U> const &right) noexcept
    {
        return qua<T>(left) += right;
    }
    template <Primitive T, Primitive U>
    [[nodiscard]] constexpr qua<T> operator-(qua<T> const &left,
                                             qua<U> const &right) noexcept
    {
        return qua<T>(left) -= right;
    }
    template <Primitive T, Primitive U>
    [[nodiscard]] constexpr qua<T> operator*(qua<T> const &left,
                                             qua<U> const &right) noexcept
    {
        return qua<T>(left) *= right;
    }
    template <Primitive T, Primitive U>
    [[nodiscard]] constexpr qua<T> operator/(qua<T> const &left,
                                             qua<U> const &right) noexcept
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
    qua<T> constexpr conjugate(qua<T> const &q)
    {
        return qua<T>(q.w, -q.x, -q.y, -q.z);
    }

    template <Primitive T>
    qua<T> constexpr inverse(qua<T> const &q)
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

    template <Primitive T>
    [[nodiscard]] constexpr qua<T> QuaternionFromEuler(vec<3, T> angles)
    {
        return qua<T>(angles.x, vec<3, T>{1, 0, 0}) *
               qua<T>(angles.y, vec<3, T>{0, 1, 0}) *
               qua<T>(angles.z, vec<3, T>{0, 0, 1});
    }
    template <Primitive T>
    [[nodiscard]] constexpr qua<T>
    QuaternionFromEuler(T roll, T pitch,
                        T yaw) requires(std::is_floating_point_v<T>)
    {
        return qua<T>(pitch, vec<3, T>{1, 0, 0}) * qua<T>(yaw, vec<3, T>{0, 1, 0}) *
               qua<T>(roll, vec<3, T>{0, 0, 1});
    }
    template <Primitive T>
    [[nodiscard]] constexpr qua<T> operator*(qua<T> const &q, qua<T> const &p)
    {
        return qua<T>(q) *= p;
    }
    template <Primitive T>
    [[nodiscard]] constexpr vec<3, T> operator*(qua<T> const &q,
                                                vec<3, T> const &v)
    {
        vec<3, T> const QuatVector(q.x, q.y, q.z);
        vec<3, T> const uv(cross(QuatVector, v));
        vec<3, T> const uuv(cross(QuatVector, uv));

        return v + ((uv * q.w) + uuv) * static_cast<T>(2);
    }

    template <Primitive T>
    [[nodiscard]] constexpr vec<3, T> operator*(vec<3, T> const &v,
                                                qua<T> const &q)
    {
        return inverse(q) * v;
    }
    template <Primitive T>
    [[nodiscard]] constexpr qua<T> conjugate(qua<T> const &q);
    template <Primitive T>
    [[nodiscard]] constexpr qua<T> inverse(qua<T> const &q);
    template <Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<3, T> cross(vec<3, T> const &left,
                                            vec<3, U> const &right);
    template <Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<3, T> cross(vec<3, U> const &left,
                                            qua<T> const &right);

    template <Primitive T>
    [[nodiscard]] constexpr mat<4, 4, T> rotate(mat<4, 4, T> const &matrix,
                                                qua<T> const &q)
    {
        return rotate(matrix, q.w, vec<3, T>{q.x, q.y, q.z});
    }

    template <Primitive T>
    [[nodiscard]] constexpr T length(qua<T> const &q)
    {
        return std::sqrt(dot(q, q));
    }
    template <Primitive T>
    [[nodiscard]] constexpr qua<T> normalize(qua<T> const &q)
    {
        T l = length(q);
        if (l <= static_cast<T>(0))
        {
            return qua<T>(1, 0, 0, 0);
        }
        T one_over_len = static_cast<T>(1) / l;
        return qua<T>(q.w * one_over_len, q.x * one_over_len, q.y * one_over_len,
                      q.z * one_over_len);
    }
} // namespace engine::math