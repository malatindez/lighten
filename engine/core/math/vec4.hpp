#pragma once

#include "vec.hpp"

#pragma warning(push)
#pragma warning(disable : 4201)
namespace engine::core::math
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
        explicit constexpr vec(std::array<T, size> const &arr) : data{ arr } {}
        explicit constexpr vec(std::array<T, size> &&arr) : data{ std::move(arr) } {}
        template <typename U>
        constexpr vec<size, T> &operator=(_detail::rvec<size, U> const &b)
        {
            for (size_t i = 0; i < size; i++)
            {
                data[i] = b.data[i];
            }
            return *this;
        }
        template <typename U>
        constexpr vec<size, T> &operator=(vec<size, U> const &b)
        {
            for (size_t i = 0; i < size; i++)
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
        constexpr vec<4, T> &operator+=(U const &other) noexcept requires(size == U::size);
        template <AnyVec U>
        constexpr vec<4, T> &operator-=(U const &other) noexcept requires(size == U::size);
        template <AnyVec U>
        constexpr vec<4, T> &operator*=(U const &other) noexcept requires(size == U::size);
        template <AnyVec U>
        constexpr vec<4, T> &operator/=(U const &other) noexcept requires(size == U::size);
        template <AnyVec U>
        constexpr vec<4, T> &operator%=(U const &other) noexcept requires(size == U::size);

        [[nodiscard]] constexpr T &operator[](size_t i);
        [[nodiscard]] constexpr T const &operator[](size_t i) const;

        union
        {
            struct
            {
                union
                {
                    T x, r, s;
                };
                union
                {
                    T y, g, t;
                };
                union
                {
                    T z, b, p;
                };
                union
                {
                    T w, a, q;
                };
            };
            std::array<T, size> data;
            vec<3, T> xyz;
            struct
            {
                T x_anonymous_padding_;
                vec<2, T> yz;
            };
            struct
            {
                vec<2, T> xy;
                vec<2, T> zw;
            };
            struct
            {
                T x_anonymous_padding__;
                vec<2, T> yzw;
            };
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
        constexpr void unpack_data(size_t offset, U u);
        template <class V>
        constexpr void unpack_data(size_t offset, V vec);
        template <typename A, typename B, typename... C>
        constexpr void unpack_data(size_t offset, A a, B b, C... c);
    };

}; // namespace engine::core::math

#pragma warning(pop)
#include "vec4.inl"