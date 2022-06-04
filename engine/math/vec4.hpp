#pragma once

#include "vec.hpp"

namespace engine::math
{

    template <Primitive T>
    struct vec<4, T>
    {
        static_assert(!std::is_const_v<T>);
        using type = T;
        static constexpr size_t size = 4;
        constexpr vec() = default;
        template <Primitive U>
        explicit constexpr vec(U value);
        template <Primitive A, Primitive B, Primitive C, Primitive D>
        explicit constexpr vec(A a, B b, C c, D d);
        template <typename... U>
        explicit constexpr vec(U... data);
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

        constexpr void reset() noexcept;

        [[nodiscard]] constexpr vec<4, T> const &operator+() const noexcept;
        [[nodiscard]] constexpr vec<4, T> operator-() const noexcept;

        template <Primitive U>
        constexpr vec<4, T> &operator+=(U const value) noexcept;
        template <Primitive U>
        constexpr vec<4, T> &operator-=(U const value) noexcept;
        template <Primitive U>
        constexpr vec<4, T> &operator*=(U const value) noexcept;
        template <Primitive U>
        constexpr vec<4, T> &operator/=(U const value) noexcept;
        template <Primitive U>
        constexpr vec<4, T> &operator%=(U const value) noexcept;
        template <AnyVec U>
        constexpr vec<4, T> &operator+=(U const &other) noexcept requires (size == U::size);
        template <AnyVec U>
        constexpr vec<4, T> &operator-=(U const &other) noexcept requires (size == U::size);
        template <AnyVec U>
        constexpr vec<4, T> &operator*=(U const &other) noexcept requires (size == U::size);
        template <AnyVec U>
        constexpr vec<4, T> &operator/=(U const &other) noexcept requires (size == U::size);
        template <AnyVec U>
        constexpr vec<4, T> &operator%=(U const &other) noexcept requires (size == U::size);

        [[nodiscard]] constexpr T &operator[](size_t i);
        [[nodiscard]] constexpr T const &operator[](size_t i) const;

        constexpr explicit operator rvec<4, T>() noexcept { return rvec<4, T>{x, y, z, w}; }
        constexpr explicit operator rvec<4, const T>() const noexcept { return rvec<4, const T>{x, y, z, w}; }

        template <size_t n = size, Primitive U = T>
        [[nodiscard]] constexpr rvec<n, U> as_rvec() noexcept requires(n >= 2 && n <= size);

        template <size_t n = size, Primitive U = T>
        [[nodiscard]] constexpr vec<n, U> as_vec() const noexcept requires(n >= 2 && n <= size);
        template <size_t n = size, Primitive U = T>
        [[nodiscard]] constexpr rvec<n, const U> as_crvec() const noexcept requires(n >= 2 && n <= size);
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

    private:
        template <Primitive _>
        static constexpr size_t get_parameter_pack_size();
        template <class V>
        static constexpr size_t get_parameter_pack_size();
        template <typename A, typename B, typename... C>
        static constexpr size_t get_parameter_pack_size();

        template <Primitive U>
        constexpr void unpack_data(int offset, U u);
        template <class V>
        constexpr void unpack_data(int offset, V vec);
        template <typename A, typename B, typename... C>
        constexpr void unpack_data(int offset, A a, B b, C... c);
    };

}; // namespace engine::math

#include "vec4.inl"