#pragma once

#include "vec.hpp"

#pragma warning( push )
#pragma warning( disable : 4201 )
namespace engine::core::math
{

    template <Primitive T>
    struct rvec<4, T>
    {
        using type = T;
        static constexpr size_t size = 4;
        explicit constexpr rvec(T &a, T &b, T &c, T &d) : x{a}, y{b}, z{c}, w{d} {}
        template <AnyVec V>
        explicit constexpr rvec(T &other) requires(V::size >= size) : x{other.x}, y{other.y}, z{other.z}, w{other.w} {}
        template <AnyVec V>
        explicit constexpr rvec(T const &other) requires(V::size >= size && std::is_const_v<typename V::type>) : x{other.x}, y{other.y}, z{other.z}, w{other.w} {}
        
        template <AnyVec U>
        static constexpr rvec<4, T> from_vec(U &other) requires (U::size >= 4) { return rvec<4, T>{other.x, other.y, other.z, other.w}; }
        template <AnyVec U>
        static constexpr rvec<4, T> from_vec(U const&other) requires (U::size >= 4) { return rvec<4, T>{other.x, other.y, other.z, other.w}; }
        

        template <typename U>
        constexpr rvec<size, T> &operator=(rvec<size, U> const &b)
        {
            for (size_t i = 0; i < size; i++)
            {
                data[i] = b.data[i];
            }
            return *this;
        }
        template <typename U>
        constexpr rvec<size, T> &operator=(vec<size, U> const &b)
        {
            for (size_t i = 0; i < size; i++)
            {
                data[i] = b.data[i];
            }
            return *this;
        }

        constexpr void reset() noexcept;

        [[nodiscard]] constexpr rvec<4, T> const &operator+() const noexcept;
        [[nodiscard]] constexpr vec<4, T> operator-() const noexcept;

        template <Primitive U>
        constexpr rvec<4, T> &operator+=(U const value) noexcept;
        template <Primitive U>
        constexpr rvec<4, T> &operator-=(U const value) noexcept;
        template <Primitive U>
        constexpr rvec<4, T> &operator*=(U const value) noexcept;
        template <Primitive U>
        constexpr rvec<4, T> &operator/=(U const value) noexcept;
        template <Primitive U>
        constexpr rvec<4, T> &operator%=(U const value) noexcept;
        template <AnyVec U>
        constexpr rvec<4, T> &operator+=(U const &other) noexcept requires(size == U::size);
        template <AnyVec U>
        constexpr rvec<4, T> &operator-=(U const &other) noexcept requires(size == U::size);
        template <AnyVec U>
        constexpr rvec<4, T> &operator*=(U const &other) noexcept requires(size == U::size);
        template <AnyVec U>
        constexpr rvec<4, T> &operator/=(U const &other) noexcept requires(size == U::size);
        template <AnyVec U>
        constexpr rvec<4, T> &operator%=(U const &other) noexcept requires(size == U::size);

        [[nodiscard]] constexpr T &operator[](size_t i);
        [[nodiscard]] constexpr T const &operator[](size_t i) const;

        constexpr explicit operator rvec<4, const T>() const noexcept { return rvec<4, const T>{x, y, z, w}; }

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
                union { _detail::primitive_reference_wrapper<T> x, r, s; };
                union { _detail::primitive_reference_wrapper<T> y, g, t; };
                union { _detail::primitive_reference_wrapper<T> z, b, p; };
                union { _detail::primitive_reference_wrapper<T> w, a, q; };
            };
            std::array<T, size> data;
        };
        static_assert(sizeof(data) == size * sizeof(T));
    };

}; // namespace engine::core::math
#pragma warning ( pop )
#include "rvec4.inl"