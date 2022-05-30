#pragma once
#include "matnxn.hpp"
#include "vec_math.hpp"
#include <cstdint>
#include <numbers>
namespace engine::math
{
    template <Primitive T>
    class qua
    {
    public:
        using type = T;
        static constexpr size_t size = 4;
        constexpr explicit qua() = default;
        constexpr explicit qua(T w, T x, T y, T z)
        {
            this->w = w;
            this->x = x;
            this->y = y;
            this->z = z;
        }
        constexpr void reset()
        {
            x = y = z = T(0);
            w = T(1);
        }

        constexpr explicit qua(T radians, vec<3, T> axis)
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

        [[nodiscard]] constexpr mat<3, 3, T> as_mat3() const noexcept
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
        [[nodiscard]] constexpr mat<4, 4, T> as_mat4() const noexcept
        {
            return mat<4, 4, T>(as_mat3());
        }
        [[nodiscard]] constexpr vec<3, T> axis() const noexcept
        {
            return vec<3, T>{x, y, z};
        }
        [[nodiscard]] constexpr T radians() const noexcept
        {
            return w;
        }

        [[nodiscard]] constexpr qua<T> const &operator+() const noexcept;
        [[nodiscard]] constexpr qua<T> operator-() const noexcept;

        template <Primitive U>
        constexpr qua<T> &operator+=(U const value) noexcept;
        template <Primitive U>
        constexpr qua<T> &operator-=(U const value) noexcept;
        template <Primitive U>
        constexpr qua<T> &operator*=(U const value) noexcept;
        template <Primitive U>
        constexpr qua<T> &operator/=(U const value) noexcept;
        template <Primitive U>
        constexpr qua<T> &operator+=(qua<U> const &other) noexcept;
        template <Primitive U>
        constexpr qua<T> &operator-=(qua<U> const &other) noexcept;
        template <Primitive U>
        constexpr qua<T> &operator*=(qua<U> const &other) noexcept;
        template <Primitive U>
        constexpr qua<T> &operator/=(qua<U> const &other) noexcept;

        [[nodiscard]] constexpr T &operator[](size_t i);
        [[nodiscard]] constexpr T const &operator[](size_t i) const;

        union
        {
            struct
            {
                union { T x, r, s; };
                union { T y, g, t; };
                union { T z, b, p; };
                union { T w, a, q; };
            };
            std::array<T, size> data;
        };
        static_assert(sizeof(data) == size * sizeof(T));
    };

    template <size_t size, Primitive T>
    constexpr std::istream &operator>>(std::istream &is, vec<size, T> &vec);
    template <size_t size, Primitive T>
    constexpr std::ostream &operator<<(std::ostream &os, vec<size, T> &vec);

    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T> operator*(U const value, vec<size, T> const &vector);
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T> operator+(vec<size, T> const &vector, U const value) noexcept;
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T> operator-(vec<size, T> const &vector, U const value) noexcept;
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T> operator*(vec<size, T> const &vector, U const value) noexcept;
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T> operator/(vec<size, T> const &vector, U const value) noexcept;
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T>
    operator+(vec<size, T> const &left, vec<size, U> const &right) noexcept;
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T>
    operator-(vec<size, T> const &left, vec<size, U> const &right) noexcept;
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T>
    operator*(vec<size, T> const &left, vec<size, U> const &right) noexcept;
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T>
    operator/(vec<size, T> const &left, vec<size, U> const &right) noexcept;

    template <Primitive T, Primitive U>
    [[nodiscard]] constexpr T dot(qua<T> const &left, qua<U> const &right);
    template <Primitive T>
    [[nodiscard]] constexpr qua<T> QuaternionFromEuler(vec<3, T> angles)
    {
        return qua<T>(angles.x, vec<3, T>{1, 0, 0}) *
               qua<T>(angles.y, vec<3, T>{0, 1, 0}) *
               qua<T>(angles.z, vec<3, T>{0, 0, 1});
    }
    template <Primitive T>
    [[nodiscard]] constexpr qua<T> QuaternionFromEuler(T roll, T pitch, T yaw) requires(std::is_floating_point_v<T>)
    {
        return qua<T>(pitch, vec<3, T>{1, 0, 0}) *
               qua<T>(yaw, vec<3, T>{0, 1, 0}) *
               qua<T>(roll, vec<3, T>{0, 0, 1});
    }
    template <Primitive T>
    [[nodiscard]] constexpr qua<T> operator*(qua<T> const &q, qua<T> const &p)
    {
        return qua<T>(q) *= p;
    }
    template <Primitive T>
    [[nodiscard]] constexpr vec<3, T> operator*(qua<T> const &q, vec<3, T> const &v)
    {
        vec<3, T> const QuatVector(q.x, q.y, q.z);
        vec<3, T> const uv(cross(QuatVector, v));
        vec<3, T> const uuv(cross(QuatVector, uv));

        return v + ((uv * q.w) + uuv) * static_cast<T>(2);
    }

    template <Primitive T>
    [[nodiscard]] constexpr vec<3, T> operator*(vec<3, T> const &v, qua<T> const &q)
    {
        return inverse(q) * v;
    }
    template <Primitive T>
    [[nodiscard]] constexpr qua<T> conjugate(qua<T> const &q);
    template <Primitive T>
    [[nodiscard]] constexpr qua<T> inverse(qua<T> const &q);
    template <Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<3, T> cross(vec<3, T> const &left, vec<3, U> const &right);
    template <Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<3, T> cross(vec<3, U> const &left, qua<T> const &right);

    template <Primitive T>
    [[nodiscard]] constexpr mat<4, 4, T> rotate(mat<4, 4, T> const &matrix, qua<T> const &q)
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
        return qua<T>(q.w * one_over_len, q.x * one_over_len, q.y * one_over_len, q.z * one_over_len);
    }
}

namespace engine::math
{
    using quat = qua<float>;
    using dquat = qua<double>;
}; // namespace engine::math
#include "quaternion.inl"