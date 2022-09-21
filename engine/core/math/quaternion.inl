#pragma once
#include "quaternion.hpp"

namespace engine::core::math
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
        if (length(axis) != 0)
        {
            axis = normalize(axis);
        }
        w = cos(radians / 2);
        auto const ss = sin(radians / 2);
        x = axis.x * ss;
        y = axis.y * ss;
        z = axis.z * ss;
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
        return_value[1][0] = T(2) * (qxy - qwz);
        return_value[2][0] = T(2) * (qxz + qwy);

        return_value[0][1] = T(2) * (qxy + qwz);
        return_value[1][1] = T(1) - T(2) * (qxx + qzz);
        return_value[2][1] = T(2) * (qyz - qwx);

        return_value[0][2] = T(2) * (qxz - qwy);
        return_value[1][2] = T(2) * (qyz + qwx);
        return_value[2][2] = T(1) - T(2) * (qxx + qyy);
        return return_value;
    }

    template <Primitive T>
    [[nodiscard]] constexpr mat<4, 4, T> qua<T>::as_mat4() const noexcept
    {
        return mat<4, 4, T>(as_mat3());
    }
    template <Primitive T>
    [[nodiscard]] constexpr vec<3, T> const &qua<T>::axis() const noexcept
    {
        return *reinterpret_cast<vec<3, T> *>((void *)&x);
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
        qua<T> const t0(*this);
        qua<T> const t1(other);

        this->w = t0.w * t1.w - t0.x * t1.x - t0.y * t1.y - t0.z * t1.z;
        this->x = t0.w * t1.x + t0.x * t1.w + t0.y * t1.z - t0.z * t1.y;
        this->y = t0.w * t1.y + t0.y * t1.w + t0.z * t1.x - t0.x * t1.z;
        this->z = t0.w * t1.z + t0.z * t1.w + t0.x * t1.y - t0.y * t1.x;
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
    [[nodiscard]] constexpr qua<T> operator*(U const value, qua<T> const &vector) noexcept
    {
        return qua<T>(vector) *= value;
    }
    template <Primitive T, Primitive U>
    [[nodiscard]] constexpr qua<T> operator/(U const value, qua<T> const &vector) noexcept
    {
        return qua<T>(value / vector.x, value / vector.y, value / vector.z, value / vector.w);
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
    constexpr T dot(qua<T> const &left, qua<U> const &right) noexcept
    {
        T return_value = 0;
        for (size_t i = 0; i < 4; i++)
        {
            return_value += left[i] * static_cast<T>(right[i]);
        }
        return return_value;
    }

    template <Primitive T>
    [[nodiscard]] constexpr T angle(qua<T> const &quaternion) noexcept
    {
        if (abs(quaternion.w) > cos(0.5f))
        {
            T const a = asin(sqrt(quaternion.x * quaternion.x +
                                  quaternion.y * quaternion.y +
                                  quaternion.z * quaternion.z))
                * static_cast<T>(2);
            if (quaternion.w < static_cast<T>(0))
            {
                return std::numbers::pi * static_cast<T>(2) - a;
            }
            return a;
        }
        return acos(quaternion.w) * T(2);
    }

    template <Primitive T>
    [[nodiscard]] constexpr T pitch(qua<T> const &quaternion) noexcept
    {
        return std::atan2(
            2 * (quaternion.y * quaternion.z + quaternion.w * quaternion.x),
            quaternion.w * quaternion.w - quaternion.x * quaternion.x - quaternion.y * quaternion.y + quaternion.z * quaternion.z);
    }
    template <Primitive T>
    [[nodiscard]] constexpr T yaw(qua<T> const &quaternion) noexcept
    {
        return std::asin(clamp(static_cast<T>(-2) * (quaternion.x * quaternion.z - quaternion.w * quaternion.y), static_cast<T>(-1), static_cast<T>(1)));
    }
    template <Primitive T>
    [[nodiscard]] constexpr T roll(qua<T> const &quaternion) noexcept
    {
        return std::atan2(
            2 * (quaternion.x * quaternion.y + quaternion.w * quaternion.z),
            quaternion.w * quaternion.w + quaternion.x * quaternion.x - quaternion.y * quaternion.y - quaternion.z * quaternion.z);
    }
    template <Primitive T>
    [[nodiscard]] constexpr vec<3, T> QuaternionToEuler(qua<T> const &quaternion) noexcept
    {
        return vec<3, T>(pitch(quaternion), yaw(quaternion), roll(quaternion));
    }

    template <Primitive T>
    qua<T> constexpr conjugate(qua<T> const &q) noexcept
    {
        return qua<T>(q.w, -q.x, -q.y, -q.z);
    }

    template <Primitive T>
    qua<T> constexpr inverse(qua<T> const &q) noexcept
    {
        return conjugate(q) / dot(q, q);
    }

    template <Primitive T, Primitive U>
    constexpr vec<3, T> cross(qua<T> const &left, vec<3, U> const &right) noexcept
    {
        return left * right;
    }
    template <Primitive T, Primitive U>
    constexpr vec<3, T> cross(vec<3, U> const &left, qua<T> const &right) noexcept
    {
        return inverse(right) * left;
    }
    template <Primitive T, Primitive U>
    [[nodiscard]] constexpr qua<T> cross(qua<T> const &left, qua<U> const &right) noexcept
    {
        return qua<T>{
            left.w *right.x + left.x * right.w + left.y * right.z - left.z * right.y,
                left.w *right.y - left.x * right.z + left.y * right.w + left.z * right.x,
                left.w *right.z + left.x * right.y - left.y * right.x + left.z * right.w,
                left.w *right.w - left.x * right.x - left.y * right.y - left.z * right.z};
    }

    template <Primitive T>
    [[nodiscard]] constexpr qua<T> QuaternionFromEuler(vec<3, T> const &angles) noexcept
    {
        vec<3, T> c = cos(angles * T(0.5));
        vec<3, T> s = sin(angles * T(0.5));

        return qua<T>{
            c.x *c.y *c.z + s.x * s.y * s.z,
                s.x *c.y *c.z - c.x * s.y * s.z,
                c.x *s.y *c.z + s.x * c.y * s.z,
                c.x *c.y *s.z - s.x * s.y * c.z
        };
    }
    template <Primitive T>
    [[nodiscard]] constexpr qua<T> QuaternionFromEuler(T roll, T pitch, T yaw) noexcept requires(std::is_floating_point_v<T>)
    {
        return QuaternionFromEuler(vec<3, T>(roll, pitch, yaw));
    }
    template <AnyMat U>
    [[nodiscard]] constexpr qua<typename U::type> QuaternionFromRotationMatrix(U const &m) noexcept requires(U::size.x == 3 && U::size.y == 3 && std::is_floating_point_v<typename U::type>)
    {
        using T = typename U::type;
        T fourXSquaredMinus1 = m[0][0] - m[1][1] - m[2][2];
        T fourYSquaredMinus1 = m[1][1] - m[0][0] - m[2][2];
        T fourZSquaredMinus1 = m[2][2] - m[0][0] - m[1][1];
        T fourWSquaredMinus1 = m[0][0] + m[1][1] + m[2][2];

        int biggestIndex = 0;
        T fourBiggestSquaredMinus1 = fourWSquaredMinus1;
        if (fourXSquaredMinus1 > fourBiggestSquaredMinus1)
        {
            fourBiggestSquaredMinus1 = fourXSquaredMinus1;
            biggestIndex = 1;
        }
        if (fourYSquaredMinus1 > fourBiggestSquaredMinus1)
        {
            fourBiggestSquaredMinus1 = fourYSquaredMinus1;
            biggestIndex = 2;
        }
        if (fourZSquaredMinus1 > fourBiggestSquaredMinus1)
        {
            fourBiggestSquaredMinus1 = fourZSquaredMinus1;
            biggestIndex = 3;
        }

        T biggestVal = std::sqrt(fourBiggestSquaredMinus1 + static_cast<T>(1)) * static_cast<T>(0.5);
        T mult = static_cast<T>(0.25) / biggestVal;

        switch (biggestIndex)
        {
        case 0:
            return qua<T>(biggestVal, (m[1][2] - m[2][1]) * mult, (m[2][0] - m[0][2]) * mult, (m[0][1] - m[1][0]) * mult);
        case 1:
            return qua<T>((m[1][2] - m[2][1]) * mult, biggestVal, (m[0][1] + m[1][0]) * mult, (m[2][0] + m[0][2]) * mult);
        case 2:
            return qua<T>((m[2][0] - m[0][2]) * mult, (m[0][1] + m[1][0]) * mult, biggestVal, (m[1][2] + m[2][1]) * mult);
        case 3:
            return qua<T>((m[0][1] - m[1][0]) * mult, (m[2][0] + m[0][2]) * mult, (m[1][2] + m[2][1]) * mult, biggestVal);
        default: // Silence a -Wswitch-default warning in GCC. Should never actually get here. Assert is just for sanity.
            assert(false);
            return qua<T>(1, 0, 0, 0);
        }
    }
    template <Primitive T>
    [[nodiscard]] constexpr qua<T> operator*(qua<T> const &q, qua<T> const &p) noexcept
    {
        return qua<T>(q) *= p;
    }
    template <Primitive T>
    [[nodiscard]] constexpr vec<3, T> operator*(qua<T> const &q, vec<3, T> const &v) noexcept
    {
        vec<3, T> const QuatVector(q.x, q.y, q.z);
        vec<3, T> const uv(cross(QuatVector, v));
        vec<3, T> const uuv(cross(QuatVector, uv));

        return v + ((uv * q.w) + uuv) * static_cast<T>(2);
    }

    template <Primitive T>
    [[nodiscard]] constexpr vec<3, T> operator*(vec<3, T> const &v, qua<T> const &q) noexcept
    {
        return inverse(q) * v;
    }

    template <Primitive T>
    [[nodiscard]] constexpr qua<T> rotate(mat<4, 4, T> const &matrix, qua<T> const &q) noexcept
    {
        return rotate(matrix, q.w, vec<3, T>{q.x, q.y, q.z});
    }


    template <Primitive T>
    [[nodiscard]] constexpr T length(qua<T> const &q) noexcept
    {
        return sqrt(dot(q, q));
    }
    template <Primitive T>
    [[nodiscard]] constexpr qua<T> normalize(qua<T> const &q) noexcept
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
} // namespace engine::core::math