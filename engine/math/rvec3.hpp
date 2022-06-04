#pragma once

#include "vec.hpp"

namespace engine::core::math
{

    template <Primitive T>
    struct rvec<3, T>
    {
        using type = T;
        static constexpr size_t size = 3;
        explicit constexpr rvec(T &a, T &b, T &c) : x{a}, y{b}, z{c} {}
        template <AnyVec T>
        explicit constexpr rvec(T &other) requires(T::size >= size) : x{other.x}, y{other.y}, z{other.z} {}
        template <AnyVec T>
        explicit constexpr rvec(T const &other) requires(T::size >= size && std::is_const_v<typename T::type>) : x{other.x}, y{other.y}, z{other.z} {}
        template <AnyVec U>
        static constexpr rvec<3, T> from_vec(U &other) requires (U::size >= 3) { return rvec<3, T>(other.x, other.y, other.z); }
        template <AnyVec U>
        static constexpr rvec<3, T> from_vec(U const&other) requires (U::size >= 3) { return rvec<3, T>(other.x, other.y, other.z); }
        constexpr void reset() noexcept;
        template <typename U>
        constexpr rvec<size, T> &operator=(rvec<size, U> const &b)
        {
            for (int i = 0; i < size; i++)
            {
                data[i] = b.data[i];
            }
            return *this;
        }
        template <typename U>
        constexpr rvec<size, T> &operator=(vec<size, U> const &b)
        {
            for (int i = 0; i < size; i++)
            {
                data[i] = b.data[i];
            }
            return *this;
        }

        [[nodiscard]] constexpr rvec<3, T> const &operator+() const noexcept;
        [[nodiscard]] constexpr vec<3, T> operator-() const noexcept;

        template <Primitive U>
        constexpr rvec<3, T> &operator+=(U const value) noexcept;
        template <Primitive U>
        constexpr rvec<3, T> &operator-=(U const value) noexcept;
        template <Primitive U>
        constexpr rvec<3, T> &operator*=(U const value) noexcept;
        template <Primitive U>
        constexpr rvec<3, T> &operator/=(U const value) noexcept;
        template <Primitive U>
        constexpr rvec<3, T> &operator%=(U const value) noexcept;
        template <AnyVec U>
        constexpr rvec<3, T> &operator+=(U const &other) noexcept requires (size == U::size);
        template <AnyVec U>
        constexpr rvec<3, T> &operator-=(U const &other) noexcept requires (size == U::size);
        template <AnyVec U>
        constexpr rvec<3, T> &operator*=(U const &other) noexcept requires (size == U::size);
        template <AnyVec U>
        constexpr rvec<3, T> &operator/=(U const &other) noexcept requires (size == U::size);
        template <AnyVec U>
        constexpr rvec<3, T> &operator%=(U const &other) noexcept requires (size == U::size);


        [[nodiscard]] constexpr T &operator[](size_t i);
        [[nodiscard]] constexpr T const &operator[](size_t i) const;

        constexpr explicit operator rvec<3, const T>() const noexcept { return rvec<3, const T>{x, y, z}; }
        
        template <size_t n = size>
        [[nodiscard]] constexpr rvec<n, T> as_rvec() noexcept requires(n >= 2 && n <= size);

        template <size_t n = size, Primitive U = T>
        [[nodiscard]] constexpr vec<n, std::remove_const_t<U>> as_vec() const noexcept requires(n >= 2 && n <= size);
        template <size_t n = size>
        [[nodiscard]] constexpr rvec<n, const T> as_crvec() const noexcept requires(n >= 2 && n <= size);
        union
        {
            struct
            {
                union {  _detail::primitive_reference_wrapper<T> x, r, s; };
                union {  _detail::primitive_reference_wrapper<T> y, g, t; };
                union {  _detail::primitive_reference_wrapper<T> z, b, p; };
            };
            std::array< _detail::primitive_reference_wrapper<T>, 3> data;
        };
        static_assert(sizeof(data) == 3 * sizeof( _detail::primitive_reference_wrapper<T>));
    };


}; // namespace engine::core::math
#include "rvec3.inl"