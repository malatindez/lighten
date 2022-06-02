#pragma once
#include "matnxn.hpp"
#include "vec_math.hpp"
namespace engine::math
{
    template <Primitive T>
    class qua
    {
    public:
        using type = T;
        static constexpr size_t size = 4;
        constexpr explicit qua() = default;
        constexpr explicit qua(T w, T x, T y, T z);
        constexpr void reset();
        constexpr explicit qua(T radians, vec<3, T> axis);
        [[nodiscard]] constexpr mat<3, 3, T> as_mat3() const noexcept;
        [[nodiscard]] constexpr mat<4, 4, T> as_mat4() const noexcept;
        [[nodiscard]] constexpr vec<3, T> axis() const noexcept;
        [[nodiscard]] constexpr T radians() const noexcept;

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
    [[nodiscard]] constexpr vec<size, T> operator*(U const value,
                                                   vec<size, T> const &vector);
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T> operator+(vec<size, T> const &vector,
                                                   U const value) noexcept;
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T> operator-(vec<size, T> const &vector,
                                                   U const value) noexcept;
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T> operator*(vec<size, T> const &vector,
                                                   U const value) noexcept;
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T> operator/(vec<size, T> const &vector,
                                                   U const value) noexcept;
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
    [[nodiscard]] constexpr qua<T> QuaternionFromEuler(vec<3, T> angles);
    template <Primitive T>
    [[nodiscard]] constexpr qua<T>
    QuaternionFromEuler(T roll, T pitch,
                        T yaw) requires(std::is_floating_point_v<T>);
    template <Primitive T>
    [[nodiscard]] constexpr qua<T> operator*(qua<T> const &q, qua<T> const &p);
    template <Primitive T>
    [[nodiscard]] constexpr vec<3, T> operator*(qua<T> const &q,
                                                vec<3, T> const &v);

    template <Primitive T>
    [[nodiscard]] constexpr vec<3, T> operator*(vec<3, T> const &v,
                                                qua<T> const &q);
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
                                                qua<T> const &q);
    template <Primitive T>
    [[nodiscard]] constexpr T length(qua<T> const &q);
    template <Primitive T>
    [[nodiscard]] constexpr qua<T> normalize(qua<T> const &q);
} // namespace engine::math

namespace engine::math
{
    using quat = qua<float>;
    using dquat = qua<double>;
}; // namespace engine::math
#include "quaternion.inl"