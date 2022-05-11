#pragma once

#include "vec.hpp"

namespace engine::math
{

    template <Primitive T>
    struct vec<3, T>
    {
        using type = T;
        static constexpr size_t size = 3;
        constexpr vec() = default;
        template<Primitive U>
        explicit constexpr vec(U value) { x = y = z = static_cast<T>(value); }
        template<Primitive A, Primitive B, Primitive C>
        explicit constexpr vec(A a, B b, C c) { x = a; y = b; z = c; }
        template <typename... U>
        explicit constexpr vec(U... data)
        {
            static_assert(get_parameter_pack_size<U...>() == size, "You have provided wrong amount of data");
            unpack_data(0, data...);
        }

        constexpr void reset() noexcept;

        [[nodiscard]] constexpr vec<3, T> const &operator+() const noexcept;
        [[nodiscard]] constexpr vec<3, T> operator-() const noexcept;

        template <Primitive U>
        constexpr vec<3, T> &operator+=(U const value) noexcept;
        template <Primitive U>
        constexpr vec<3, T> &operator-=(U const value) noexcept;
        template <Primitive U>
        constexpr vec<3, T> &operator*=(U const value) noexcept;
        template <Primitive U>
        constexpr vec<3, T> &operator/=(U const value) noexcept;
        template <Primitive U>
        constexpr vec<3, T> &operator+=(vec<3, U> const &other) noexcept;
        template <Primitive U>
        constexpr vec<3, T> &operator-=(vec<3, U> const &other) noexcept;
        template <Primitive U>
        constexpr vec<3, T> &operator*=(vec<3, U> const &other) noexcept;
        template <Primitive U>
        constexpr vec<3, T> &operator/=(vec<3, U> const &other) noexcept;

        [[nodiscard]] constexpr T &operator[](size_t i);
        [[nodiscard]] constexpr T const &operator[](size_t i) const;
        union
        {
            struct
            {
                union { T x, r, s; };
                union { T y, g, t; };
                union { T z, b, p; };
            };
            std::array<T, 3> data;
        };
        static_assert(sizeof(data) == 3 * sizeof(T));
        
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

    template <Primitive T, Primitive U>
    constexpr vec<3, T> cross(vec<3, T> const &left, vec<3, U> const &right);

}; // namespace engine::math
#include "vec3.inl"