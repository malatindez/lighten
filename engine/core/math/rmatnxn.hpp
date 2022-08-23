#pragma once
#include "mat.hpp"
#pragma warning(push)
#pragma warning(disable : 4201)
namespace engine::core::math
{

    template <size_t a, size_t b, Primitive T>
    struct rmat
    {
        using type = T;
        static constexpr ivec2 size { a, b };
        constexpr rmat() {}
        template <size_t c, size_t d, Primitive P>
        constexpr rmat(mat<c, d, P> &p) requires(c >= a && d >= b);
        template <size_t c, size_t d, Primitive P>
        constexpr rmat(rmat<c, d, P> &p) requires(c >= a && d >= b);

        template <typename U>
        constexpr rmat<a, b, T> &operator=(mat<a, b, U> const &mat)
        {
            for (int i = 0; i < size.x * size.y; i++)
            {
                arr[i] = mat.arr[i];
            }
            return *this;
        }
        template <typename U>
        constexpr rmat<a, b, T> &operator=(rmat<a, b, U> const &mat)
        {
            for (int i = 0; i < size.x * size.y; i++)
            {
                arr[i] = mat.arr[i];
            }
            return *this;
        }

        // sets all values to zero
        constexpr void reset() noexcept;

        [[nodiscard]] constexpr _detail::rvec<b, T> &operator[](size_t i);
        [[nodiscard]] constexpr _detail::rvec<b, T> const &operator[](size_t i) const;
        [[nodiscard]] constexpr rmat<a, b, T> const &operator+() const noexcept;
        [[nodiscard]] constexpr mat<a, b, T> operator-() const noexcept;

        template <Primitive U>
        constexpr rmat<a, b, T> &operator+=(rmat<a, b, U> const &other);
        template <Primitive U>
        constexpr rmat<a, b, T> &operator-=(rmat<a, b, U> const &other);
        template <size_t c, Primitive U>
        constexpr rmat<a, c, T> &operator*=(rmat<b, c, U> const &other);

        template <Primitive U>
        constexpr rmat<a, b, T> &operator+=(mat<a, b, U> const &other);
        template <Primitive U>
        constexpr rmat<a, b, T> &operator-=(mat<a, b, U> const &other);
        template <size_t c, Primitive U>
        constexpr rmat<a, c, T> &operator*=(mat<b, c, U> const &other);

        template <Primitive U>
        constexpr rmat<a, b, T> &operator+=(U const value);
        template <Primitive U>
        constexpr rmat<a, b, T> &operator-=(U const value);
        template <Primitive U>
        constexpr rmat<a, b, T> &operator*=(U const value);

        union
        {
            std::array<_detail::primitive_reference_wrapper<T>, size.x *size.y> arr;
            std::array<_detail::rvec<size.y, T>, size.x> data;
        };
        static_assert(sizeof(arr) == sizeof(data));
    };
} // namespace engine::core::math
#pragma warning(pop)
#include "rmatnxn.inl"