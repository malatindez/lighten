#pragma once

#include "vec.hpp"
namespace engine::math
{

    template <size_t L, Primitive T>
    struct vec
    {
        using type = T;
        static constexpr size_t size = L;
        constexpr vec() = default;
        template <Primitive U>
        explicit constexpr vec(U value)
        {
            for (int i = 0; i < size; i++)
            {
                data[i] = static_cast<T>(value);
            }
        }
        template <typename... U>
        explicit constexpr vec(U... data)
        {
            static_assert(get_parameter_pack_size<U...>() == size, "You have provided wrong amount of data");
            unpack_data(0, data...);
        }

        constexpr void reset() noexcept;

        template <Primitive U>
        constexpr vec<L, T> &operator+=(U const value) noexcept;
        template <Primitive U>
        constexpr vec<L, T> &operator-=(U const value) noexcept;
        template <Primitive U>
        constexpr vec<L, T> &operator*=(U const value) noexcept;
        template <Primitive U>
        constexpr vec<L, T> &operator/=(U const value) noexcept;
        template <Primitive U>
        constexpr vec<L, T> &operator+=(vec<L, U> const &other) noexcept;
        template <Primitive U>
        constexpr vec<L, T> &operator-=(vec<L, U> const &other) noexcept;
        template <Primitive U>
        constexpr vec<L, T> &operator*=(vec<L, U> const &other) noexcept;
        template <Primitive U>
        constexpr vec<L, T> &operator/=(vec<L, U> const &other) noexcept;

        [[nodiscard]] constexpr T &operator[](size_t i);
        [[nodiscard]] constexpr T const &operator[](size_t i) const;
        std::array<T, size> data;

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
#include "vecn.inl"