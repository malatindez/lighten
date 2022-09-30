#pragma once
#include "mat.hpp"
#pragma warning(push)
#pragma warning(disable : 4201)

namespace engine::core::math
{
    template <Primitive T>
    struct qua
    {
        using type = T;
        static constexpr size_t size = 4;
        constexpr qua() : qua(1, 0, 0, 0) {}
        constexpr explicit qua(T w, T x, T y, T z);
        constexpr void reset();
        constexpr explicit qua(T radians, vec<3, T> axis);
        [[nodiscard]] constexpr mat<3, 3, T> as_mat3() const noexcept;
        [[nodiscard]] constexpr mat<4, 4, T> as_mat4() const noexcept;
        [[nodiscard]] constexpr vec<3, T> const &axis() const noexcept;
        [[nodiscard]] constexpr T radians() const noexcept;

        [[nodiscard]] constexpr qua<T> const &operator+() const noexcept;
        [[nodiscard]] constexpr qua<T> operator-() const noexcept;

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
            std::array<T, size> data;
        };
        static_assert(sizeof(data) == size * sizeof(T));
    };

    template <size_t size, Primitive T>
    constexpr std::istream &operator>>(std::istream &is, vec<size, T> &vec);
    template <size_t size, Primitive T>
    constexpr std::ostream &operator<<(std::ostream &os, vec<size, T> &vec);

    template <Primitive T, Primitive U>
    [[nodiscard]] constexpr qua<T> operator*(U const value, qua<T> const &vector) noexcept;
    template <Primitive T, Primitive U>
    [[nodiscard]] constexpr qua<T> operator/(U const value, qua<T> const &vector) noexcept;
    template <Primitive T, Primitive U>
    [[nodiscard]] constexpr qua<T> operator*(qua<T> const &vector, U const value) noexcept;
    template <Primitive T, Primitive U>
    [[nodiscard]] constexpr qua<T> operator/(qua<T> const &vector, U const value) noexcept;
    template <Primitive T, Primitive U>
    [[nodiscard]] constexpr qua<T> operator+(qua<T> const &left, qua<U> const &right) noexcept;
    template <Primitive T, Primitive U>
    [[nodiscard]] constexpr qua<T> operator-(qua<T> const &left, qua<U> const &right) noexcept;
    template <Primitive T, Primitive U>
    [[nodiscard]] constexpr qua<T> operator*(qua<T> const &left, qua<U> const &right) noexcept;
    template <Primitive T, Primitive U>
    [[nodiscard]] constexpr qua<T> operator/(qua<T> const &left, qua<U> const &right) noexcept;

    template <Primitive T, Primitive U>
    [[nodiscard]] constexpr T dot(qua<T> const &left, qua<U> const &right) noexcept;
    template <Primitive T>
    [[nodiscard]] constexpr T angle(qua<T> const &quaternion) noexcept;
    template <Primitive T>
    [[nodiscard]] constexpr T pitch(qua<T> const &quaternion) noexcept;
    template <Primitive T>
    [[nodiscard]] constexpr T yaw(qua<T> const &quaternion) noexcept;
    template <Primitive T>
    [[nodiscard]] constexpr T roll(qua<T> const &quaternion) noexcept;
    template <Primitive T>
    [[nodiscard]] constexpr vec<3, T> QuaternionToEuler(qua<T> const &quaternion) noexcept;

    template <Primitive T>
    [[nodiscard]] constexpr qua<T> QuaternionFromEuler(vec<3, T> const &angles) noexcept;
    template <Primitive T>
    [[nodiscard]] constexpr qua<T> QuaternionFromEuler(T roll, T pitch, T yaw) noexcept requires(std::is_floating_point_v<T>);
    template <AnyMat U>
    [[nodiscard]] constexpr qua<typename U::type> QuaternionFromRotationMatrix(U const &mat) noexcept requires(U::size.x == 3 && U::size.y == 3 && std::is_floating_point_v<typename U::type>);
    template <Primitive T>
    [[nodiscard]] constexpr qua<T> operator*(qua<T> const &q, qua<T> const &p) noexcept;
    template <Primitive T>
    [[nodiscard]] constexpr vec<3, T> operator*(qua<T> const &q, vec<3, T> const &v) noexcept;

    template <Primitive T>
    [[nodiscard]] constexpr vec<3, T> operator*(vec<3, T> const &v, qua<T> const &q) noexcept;
    template <Primitive T>
    [[nodiscard]] constexpr qua<T> conjugate(qua<T> const &q) noexcept;
    template <Primitive T>
    [[nodiscard]] constexpr qua<T> inverse(qua<T> const &q) noexcept;
    template <Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<3, T> cross(qua<T> const &left, vec<3, U> const &right) noexcept;
    template <Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<3, T> cross(vec<3, U> const &left, qua<T> const &right) noexcept;
    template <Primitive T, Primitive U>
    [[nodiscard]] constexpr qua<T> cross(qua<T> const &left, qua<U> const &right) noexcept;

    template <Primitive T>
    [[nodiscard]] constexpr mat<4, 4, T> rotate(mat<4, 4, T> const &matrix, qua<T> const &q) noexcept;
    template <Primitive T>
    [[nodiscard]] constexpr T length(qua<T> const &q) noexcept;
    template <Primitive T>
    [[nodiscard]] constexpr qua<T> normalize(qua<T> const &q) noexcept;
} // namespace engine::core::math

#pragma warning(pop)

namespace engine::core::math
{
    using quat = qua<float>;
    using dquat = qua<double>;
}; // namespace engine::core::math
#include "quaternion.inl"