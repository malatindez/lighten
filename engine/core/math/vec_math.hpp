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
    [[nodiscard]] constexpr vec<size, T> operator+(vec<size, T> const &vector, U const value) noexcept;
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T> operator-(vec<size, T> const &vector, U const value) noexcept;
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T> operator*(vec<size, T> const &vector, U const value) noexcept;
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T> operator/(vec<size, T> const &vector, U const value) noexcept;

    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T> operator*(U const value, vec<size, T> const &vector) noexcept;
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T> operator+(U const value, vec<size, T> const &vector) noexcept;
    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, T> operator-(U const value, vec<size, T> const &vector) noexcept;

    template <size_t size, Primitive T, Primitive U>
    [[nodiscard]] constexpr vec<size, std::remove_const_t<T>> operator*(U const value, rvec<size, T> const &vector) noexcept;
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
    [[nodiscard]] constexpr bool operator==(T const &left, U const &right) noexcept requires(T::size == U::size);

    template <AnyVec T, AnyVec U>
    [[nodiscard]] constexpr bool operator!=(T const &left, U const &right) noexcept requires(T::size == U::size);

    template <AnyVec T>
    [[nodiscard]] constexpr typename std::remove_const_t<typename T::type> squared_length(T const &vector) noexcept;
    template <AnyVec T>
    [[nodiscard]] constexpr auto length(T const &vector) noexcept;

    template <AnyVec T>
    [[nodiscard]] constexpr vec<T::size, std::remove_const_t<typename T::type>> unit_vector(T const &vector) noexcept;
    template <AnyVec T>
    [[nodiscard]] constexpr vec<T::size, std::remove_const_t<typename T::type>> normalize(T const &vector) noexcept;

    template <AnyVec T, AnyVec U>
    constexpr std::remove_const_t<typename T::type> dot(T const &left, U const &right) noexcept requires(T::size == U::size);

    template <AnyVec T, AnyVec U>
    constexpr std::remove_const_t<typename T::type> angle(T const &left, U const &right) noexcept requires(T::size == U::size);

    template <AnyVec T>
    constexpr vec<T::size, std::remove_const_t<typename T::type>> cos(T const &vector) noexcept;

    template <AnyVec T>
    constexpr vec<T::size, std::remove_const_t<typename T::type>> sin(T const &vector) noexcept;

    template <AnyVec T, Primitive U>
    constexpr vec<T::size, std::remove_const_t<typename T::type>> pow(T const &vector, U const pow) noexcept;

    template <AnyVec T, AnyVec U>
    constexpr vec<T::size, std::remove_const_t<typename T::type>> pow(T const &vector1, U const &vector2) noexcept;

    template <AnyVec T>
    constexpr vec<T::size, std::remove_const_t<typename T::type>> exp(T const &vector2) noexcept;

    template <AnyVec T>
    constexpr vec<T::size, std::remove_const_t<typename T::type>> sqrt(T const &vector) noexcept;

    template <AnyVec T, AnyVec U>
    constexpr vec<3, std::remove_const_t<typename T::type>> cross(T const &left, U const &right) noexcept requires(T::size == U::size && T::size == 3);

    template <Primitive T, Primitive U>
    constexpr void rclamp(T &left, U const min, U const max) noexcept;

    template <AnyVec T, AnyVec U>
    constexpr void rclamp(T &left, U const min, U const max) noexcept;

    template <AnyVec T, Primitive U>
    [[nodiscard]] constexpr vec<T::size, std::remove_const_t<typename T::type>> clamp(T const &left, U const min, U const max) noexcept;

    template <AnyVec T, AnyVec U>
    [[nodiscard]] constexpr vec<T::size, std::remove_const_t<typename T::type>> clamp(T const &left, U const &min, U const &max) noexcept;

    template <Primitive T, Primitive U>
    [[nodiscard]] constexpr T clamp(T left, U const min, U const max) noexcept;

    template <typename T, Primitive U>
    constexpr auto lerp(T const from, T const to, U const param) noexcept;

    template <AnyVec T, Primitive U>
    [[nodiscard]] constexpr vec<T::size, std::remove_const_t<typename T::type>> min(T const &left, U const min) noexcept;
    
    template <AnyVec T, Primitive U>
    [[nodiscard]] constexpr vec<T::size, std::remove_const_t<typename T::type>> max(T const &left, U const max) noexcept;
} // namespace engine::core::math
#include "vec_math.inl"