#pragma once

#include "vec.hpp"

namespace engine::math
{

    template <Primitive T>
    struct vec<4, T>
    {
        using type = T;
        static constexpr size_t size = 4;
        constexpr vec() = default;
        template <Primitive U>
        explicit constexpr vec(U value) { x = y = z = w = static_cast<T>(value); }
        template <Primitive A, Primitive B, Primitive C, Primitive D>
        explicit constexpr vec(A a, B b, C c, D d)
        {
            x = a;
            y = b;
            z = c;
            w = d;
        }
        template <typename... U>
        explicit constexpr vec(U... data)
        {
            static_assert(get_parameter_pack_size<U...>() == size,
                          "You have provided wrong amount of data");
            unpack_data(0, data...);
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
        template <Primitive U>
        constexpr vec<4, T> &operator+=(vec<4, U> const &other) noexcept;
        template <Primitive U>
        constexpr vec<4, T> &operator-=(vec<4, U> const &other) noexcept;
        template <Primitive U>
        constexpr vec<4, T> &operator*=(vec<4, U> const &other) noexcept;
        template <Primitive U>
        constexpr vec<4, T> &operator/=(vec<4, U> const &other) noexcept;
        template <Primitive U>
        constexpr vec<4, T> &operator%=(vec<4, U> const &other) noexcept;

        template <size_t n, Primitive U = T>
        [[nodiscard]] constexpr vec<n, U> as_vec() requires(n >= 2 && n <= size)
        {
            vec<n, U> rv;
            for (int i = 0; i < n; i++)
            {
                rv.data[i] = static_cast<U>(data[i]);
            }
            return rv;
        }
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