#pragma once
#include "vec.hpp"
namespace engine::math
{
    template <size_t size, Primitive T>
    constexpr std::istream &operator>>(std::istream &is, vec<size, T> &vec);
    template <size_t size, Primitive T>
    constexpr std::ostream &operator<<(std::ostream &os, vec<size, T> const &vec);
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
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T> operator%(vec<size, T> const &left,
                                                   U const value) noexcept;
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr bool operator%(vec<size, T> const &left,
                                           vec<size, U> const &right) noexcept;
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr bool operator==(vec<size, T> const &left,
                                            vec<size, U> const &right) noexcept;
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr bool operator!=(vec<size, T> const &left,
                                            vec<size, U> const &right) noexcept;

    template <size_t size, Primitive T>
    [[nodiscard]] constexpr float
    squared_length(vec<size, T> const &vector) noexcept;
    template <size_t size, Primitive T>
    [[nodiscard]] constexpr float length(vec<size, T> const &vector) noexcept;

    template <size_t size, Primitive T>
    [[nodiscard]] constexpr vec<size, T>
    unit_vector(vec<size, T> const &vector) noexcept;
    template <size_t size, Primitive T>
    [[nodiscard]] constexpr vec<size, T>
    normalize(vec<size, T> const &vector) noexcept;

    template <size_t size, Primitive T, Primitive U>
    constexpr T dot(vec<size, T> const &left, vec<size, U> const &right);

    template <size_t size, Primitive T, Primitive U>
    constexpr T angle(vec<size, T> const &left, vec<size, U> const &right);

    template <size_t size, Primitive T>
    constexpr vec<size, T> cos(vec<size, T> const &vector);

    template <size_t size, Primitive T>
    constexpr vec<size, T> sin(vec<size, T> const &vector);
} // namespace engine::math
#include "vec_math.inl"