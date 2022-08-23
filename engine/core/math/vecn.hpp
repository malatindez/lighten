#pragma once

#include "vec.hpp"
namespace engine::core::math
{

    template <size_t L, Primitive T>
    struct vec
    {
        static_assert(!std::is_const_v<T>);
        using type = T;
        static constexpr size_t size = L;
        constexpr vec() = default;
        template <Primitive U>
        explicit constexpr vec(U value);
        template <typename... U>
        explicit constexpr vec(U... data);

        explicit constexpr vec(std::array<T, size> const &arr) : data { arr } {}
        explicit constexpr vec(std::array<T, size> &&arr) : data { std::move(arr) } {}

        constexpr void reset() noexcept;

        template <typename U>
        constexpr vec<size, T> &operator=(vec<size, U> const &b)
        {
            for (int i = 0; i < size; i++)
            {
                data[i] = b.data[i];
            }
            return *this;
        }

        template <Primitive U>
        constexpr vec<L, T> &operator+=(U const value) noexcept;
        template <Primitive U>
        constexpr vec<L, T> &operator-=(U const value) noexcept;
        template <Primitive U>
        constexpr vec<L, T> &operator*=(U const value) noexcept;
        template <Primitive U>
        constexpr vec<L, T> &operator/=(U const value) noexcept;
        template <Primitive U>
        constexpr vec<L, T> &operator%=(U const value) noexcept;
        template <AnyVec U>
        constexpr vec<L, T> &operator+=(U const &other) noexcept requires(size == U::size);
        template <AnyVec U>
        constexpr vec<L, T> &operator-=(U const &other) noexcept requires(size == U::size);
        template <AnyVec U>
        constexpr vec<L, T> &operator*=(U const &other) noexcept requires(size == U::size);
        template <AnyVec U>
        constexpr vec<L, T> &operator/=(U const &other) noexcept requires(size == U::size);
        template <AnyVec U>
        constexpr vec<L, T> &operator%=(U const &other) noexcept requires(size == U::size);

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

}; // namespace engine::core::math
#include "vecn.inl"