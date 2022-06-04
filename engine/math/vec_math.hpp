#pragma once
#include "vec.hpp"
namespace engine::core::math

{
    template <AnyVec U>
    constexpr std::istream &operator>>(std::istream &is, U &vec);
    template <AnyVec U>
    constexpr std::ostream &operator<<(std::ostream &os, U const &vec);
    template <AnyVec U>
    constexpr std::ostream &operator<<(std::ostream &os, U &vec);

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
    [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator*(U const value, rvec<size, T> const &vector);
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator+(rvec<size, T> const &vector, U const value) noexcept;
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator-(rvec<size, T> const &vector, U const value) noexcept;
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator*(rvec<size, T> const &vector, U const value) noexcept;
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator/(rvec<size, T> const &vector, U const value) noexcept;
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator%(rvec<size, T> const &left, U const value) noexcept;


    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T> operator+(vec<size, T> const &left, vec<size, U> const &right) noexcept;
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T> operator-(vec<size, T> const &left, vec<size, U> const &right) noexcept;
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T> operator*(vec<size, T> const &left, vec<size, U> const &right) noexcept;
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T> operator/(vec<size, T> const &left, vec<size, U> const &right) noexcept;
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T> operator%(vec<size, T> const &left, U const value) noexcept;
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T> operator%(vec<size, T> const &left, vec<size, U> const &right) noexcept;

    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator+(rvec<size, T> const &left, vec<size, U> const &right) noexcept;
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator-(rvec<size, T> const &left, vec<size, U> const &right) noexcept;
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator*(rvec<size, T> const &left, vec<size, U> const &right) noexcept;
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator/(rvec<size, T> const &left, vec<size, U> const &right) noexcept;
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator%(rvec<size, T> const &left, vec<size, U> const &right) noexcept;

    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator+(rvec<size, T> const &left, rvec<size, U> const &right) noexcept;
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator-(rvec<size, T> const &left, rvec<size, U> const &right) noexcept;
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator*(rvec<size, T> const &left, rvec<size, U> const &right) noexcept;
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator/(rvec<size, T> const &left, rvec<size, U> const &right) noexcept;
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator%(rvec<size, T> const &left, rvec<size, U> const &right) noexcept;

    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T> operator+(vec<size, T> const &left, rvec<size, U> const &right) noexcept;
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T> operator-(vec<size, T> const &left, rvec<size, U> const &right) noexcept;
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T> operator*(vec<size, T> const &left, rvec<size, U> const &right) noexcept;
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T> operator/(vec<size, T> const &left, rvec<size, U> const &right) noexcept;
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T> operator%(vec<size, T> const &left, rvec<size, U> const &right) noexcept;

    template <AnyVec T, AnyVec U>
    [[nodiscard]] constexpr bool operator==(T const &left, U const &right) requires(T::size == U::size);
    template <AnyVec T, AnyVec U>
    [[nodiscard]] constexpr bool operator!=(T const &left, U const &right) requires(T::size == U::size);

    template <AnyVec T>
    [[nodiscard]] constexpr typename T::type squared_length(T const &vector) noexcept;
    template <AnyVec T>
    [[nodiscard]] constexpr typename T::type length(T const &vector) noexcept;

    template <AnyVec T>
    [[nodiscard]] constexpr vec<T::size, typename T::type> unit_vector(T const &vector) noexcept;
    template <AnyVec T>
    [[nodiscard]] constexpr vec<T::size, typename T::type> normalize(T const &vector) noexcept;

    template <AnyVec T, AnyVec U>
    constexpr typename T::type dot(T const &left, U const &right) requires(T::size == U::size);

    template <AnyVec T, AnyVec U>
    constexpr typename T::type angle(T const &left, U const &right) requires(T::size == U::size);

    template <AnyVec T>
    constexpr vec<T::size, typename T::type> cos(T const &vector);

    template <AnyVec T>
    constexpr vec<T::size, typename T::type> sin(T const &vector);

    template <AnyVec T, AnyVec U>
    constexpr vec<3, T> cross(T const &left, U const &right) requires(T::size == U::size && T::size == 3);

} // namespace engine::core::math
#include "vec_math.inl"