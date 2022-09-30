#pragma once
#include "mat.hpp"
#pragma warning(push)
#pragma warning(disable : 4201)
namespace engine::core::math
{
    template <size_t a, size_t b, Primitive T>
    struct mat
    {
        using type = T;
        static constexpr vec<2, size_t> size{ a, b };
        static constexpr mat<a, b, T> identity() noexcept
        {
            mat<a, b, T> result;
            for (size_t i = 0; i < a; i++)
            {
                for (size_t j = 0; j < b; j++)
                {
                    result.data[i][j] = i == j ? static_cast<T>(1) : static_cast<T>(0);
                }
            }
            return result;
        }

        constexpr mat() = default;
        template <Primitive P>
        explicit constexpr mat(P p) requires(a == b);
        template <size_t c, size_t d, Primitive P>
        explicit constexpr mat(mat<c, d, P> p) requires(a >= c && b >= d);
        template <size_t c, size_t d, Primitive P>
        explicit constexpr mat(rmat<c, d, P> p) requires(a >= c && b >= d);
        template <Primitive P>
        explicit constexpr mat(mat<a, b, P> p);
        template <Primitive P>
        explicit constexpr mat(rmat<a, b, P> p);
        template <typename... U>
        explicit constexpr mat(U... data);
        // sets all values to zero
        constexpr void reset() noexcept;

        [[nodiscard]] constexpr vec<b, T> &operator[](size_t i);
        [[nodiscard]] constexpr vec<b, T> const &operator[](size_t i) const;
        [[nodiscard]] constexpr mat<a, b, T> const &operator+() const noexcept;
        [[nodiscard]] constexpr mat<a, b, T> operator-() const noexcept;

        template <Primitive U>
        constexpr mat<a, b, T> &operator+=(mat<a, b, U> const &other);
        template <Primitive U>
        constexpr mat<a, b, T> &operator-=(mat<a, b, U> const &other);
        template <size_t c, Primitive U>
        constexpr mat<a, c, T> &operator*=(mat<b, c, U> const &other);

        template <Primitive U>
        constexpr mat<a, b, T> &operator+=(rmat<a, b, U> const &other);
        template <Primitive U>
        constexpr mat<a, b, T> &operator-=(rmat<a, b, U> const &other);
        template <size_t c, Primitive U>
        constexpr mat<a, c, T> &operator*=(rmat<b, c, U> const &other);

        template <Primitive U>
        constexpr mat<a, b, T> &operator+=(U const value);
        template <Primitive U>
        constexpr mat<a, b, T> &operator-=(U const value);
        template <Primitive U>
        constexpr mat<a, b, T> &operator*=(U const value);

        template <size_t c = a, size_t d = b>
        constexpr rmat<c, d, T> as_rmat() noexcept { return rmat<c, d, T>{*this}; }

        union
        {
            std::array<T, size.x *size.y> arr;
            std::array<vec<size.y, T>, size.x> data;
        };
        static_assert(sizeof(arr) == sizeof(data));

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
        constexpr void unpack_data(size_t offset, A, B, C...);
    };
} // namespace engine::core::math
#pragma warning(pop)
#include "matnxn.inl"