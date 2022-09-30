#pragma once
#include "mat.hpp"
namespace engine::core::math
{
    template <AnyMat U>
    constexpr std::istream &operator>>(std::istream &is, U &matrix);
    template <AnyMat U>
    constexpr std::ostream &operator<<(std::ostream &os, U const &matrix);

    template <AnyMat U, AnyMat V>
    [[nodiscard]] constexpr auto operator*(U const &lhs, V const &rhs) requires (U::size.x == V::size.y);
    template <AnyVec V, AnyMat M>
    [[nodiscard]] constexpr auto operator*(V const &left, M const &right) requires (V::size == M::size.y);
    template <AnyMat M, AnyVec V>
    [[nodiscard]] constexpr auto operator*(M const &left, V const &right) requires (M::size.x == V::size);
    template <AnyMat T, Primitive U>
    [[nodiscard]] constexpr mat<T::size.x, T::size.y, std::remove_const_t<typename T::type>> operator*(T const &left, U const right);
    template <AnyMat T, Primitive U>
    [[nodiscard]] constexpr mat<T::size.x, T::size.y, std::remove_const_t<typename T::type>> operator*(U const left, T const &right);
    template <AnyMat T, Primitive U>
    [[nodiscard]] constexpr mat<T::size.x, T::size.y, std::remove_const_t<typename T::type>> operator-(T const &left, U const right);
    template <AnyMat T, Primitive U>
    [[nodiscard]] constexpr mat<T::size.x, T::size.y, std::remove_const_t<typename T::type>> operator+(U const left, T const &right);
    template <AnyMat T, AnyMat U>
    [[nodiscard]] constexpr mat<T::size.x, T::size.y, std::remove_const_t<typename T::type>> operator-(T const left, U const &right) requires(T::size.x == U::size.x && T::size.y == U::size.y);
    template <AnyMat T, AnyMat U>
    [[nodiscard]] constexpr mat<T::size.x, T::size.y, std::remove_const_t<typename T::type>> operator+(T const &left, U const &right) requires(T::size.x == U::size.x && T::size.y == U::size.y);

    template <AnyMat T>
    constexpr mat<T::size.y, T::size.x, std::remove_const_t<typename T::type>> transpose(T const &matrix);
    template <AnyMat T>
    constexpr rmat<T::size.y, T::size.x, typename T::type> rtranspose(T &matrix);
    template <AnyMat T>
    constexpr rmat<T::size.y, T::size.x, const typename T::type> rctranspose(T const &matrix);

    template <AnyMat T>
    constexpr typename T::type det(T const &matrix) requires(T::size.x == T::size.y);
    template <AnyMat T>
    constexpr typename T::type determinant(T const &matrix) requires(T::size.x == T::size.y);

    template <AnyMat T>
    constexpr mat<T::size.x, T::size.y, std::remove_const_t<typename T::type>> adj(T const &m) requires(T::size.x == T::size.y);
    template <AnyMat T>
    constexpr mat<T::size.x, T::size.y, std::remove_const_t<typename T::type>> adjugate(T const &m) requires(T::size.x == T::size.y);

    template <AnyMat T>
    constexpr mat<T::size.x, T::size.y, std::remove_const_t<typename T::type>> inverse(T const &matrix) requires(T::size.x == T::size.y);

    template <AnyMat T, AnyVec V>
    constexpr mat<4, 4, std::remove_const_t<typename T::type>> translate(T const &matrix, V const &vec) requires(T::size.x == T::size.y && T::size.x == 4 && V::size == 3);

    template <AnyMat T, Primitive U, AnyVec V>
    constexpr mat<4, 4, std::remove_const_t<typename T::type>> rotate(T const &matrix, U angle, V const &axis) requires(T::size.x == T::size.y && T::size.x == 4 && V::size == 3);

    template <AnyMat T, AnyVec V>
    constexpr mat<4, 4, std::remove_const_t<typename T::type>> scale(T const &matrix, V const &scale) requires(T::size.x == T::size.y && T::size.x == 4 && V::size == 3);

    template <AnyVec Position>
    constexpr mat<4, 4, std::remove_const_t<typename Position::type>> look_at(Position const &eye, Position const &center, Position const &up) requires(Position::size == 3);

    template <Primitive T>
    constexpr mat<4, 4, T> perspective(T fov_y, T aspect_ratio, T z_near, T z_far);

    template <Primitive T>
    constexpr mat<4, 4, T> ortho(T left, T right, T bottom, T top, T zNear, T zFar);

    template <Primitive T>
    constexpr void invert_orthonormal(mat<4, 4, T> const &src, mat<4, 4, T> &dst);
    template <Primitive T>
    constexpr void invert_orthogonal(mat<4, 4, T> const &src, mat<4, 4, T> &dst);
    template <Primitive T>
    constexpr mat<4, 4, T> invert_orthonormal(mat<4, 4, T> const &src);
    template <Primitive T>
    constexpr mat<4, 4, T> invert_orthogonal(mat<4, 4, T> const &src);

} // namespace engine::core::math
#include "mat_math.inl"