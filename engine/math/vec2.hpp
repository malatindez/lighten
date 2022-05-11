#pragma once

#include "vec.hpp"

namespace engine::math
{

    template <Primitive T>
    struct vec<2, T>
    {
        using type = T;
        static constexpr size_t size = 2;
        constexpr vec() = default;
        template <Primitive U>
        explicit constexpr vec(U value) { x = y = static_cast<T>(value); }
        template<Primitive A, Primitive B>
        explicit constexpr vec(A a, B b) { x = a; y = b; }
        template <typename... U>
        explicit constexpr vec(U... data)
        {
            static_assert(get_parameter_pack_size<U...>() == size, "You have provided wrong amount of data");
            unpack_data(0, data...);
        }

        constexpr void reset() noexcept;

        [[nodiscard]] constexpr vec<2, T> const &operator+() const noexcept;

        [[nodiscard]] constexpr vec<2, T> operator-() const noexcept;

        template <Primitive U>
        constexpr vec<2, T> &operator+=(U const value) noexcept;
        template <Primitive U>
        constexpr vec<2, T> &operator-=(U const value) noexcept;
        template <Primitive U>
        constexpr vec<2, T> &operator*=(U const value) noexcept;
        template <Primitive U>
        constexpr vec<2, T> &operator/=(U const value) noexcept;
        template <Primitive U>
        constexpr vec<2, T> &operator+=(vec<2, U> const &other) noexcept;
        template <Primitive U>
        constexpr vec<2, T> &operator-=(vec<2, U> const &other) noexcept;
        template <Primitive U>
        constexpr vec<2, T> &operator*=(vec<2, U> const &other) noexcept;
        template <Primitive U>
        constexpr vec<2, T> &operator/=(vec<2, U> const &other) noexcept;
        [[nodiscard]] constexpr T &operator[](size_t i);
        [[nodiscard]] constexpr T const &operator[](size_t i) const;
        union
        {
            struct
            {
                union { T x, r, s; };
                union { T y, g, t; };
            };
            std::array<T, 2> data;
        };
        static_assert(sizeof(data) == 2 * sizeof(T));

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

#include "vec2.inl"