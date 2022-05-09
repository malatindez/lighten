#pragma once
#include "mat.hpp"
namespace engine::math
{
    template <size_t a, size_t b, Primitive T>
    struct mat
    {
        constexpr mat() = default;
        explicit constexpr mat(T value)
        {
            reset();
            for (int i = 0; i < size.x; i++)
            {
                for (int j = 0; j < size.y; j++)
                {
                    data[i][j] = value;
                }
            }
        }
        // sets all values to zero
        void reset()
        {
            for (int i = 0; i < size.x; i++)
            {
                data[i].reset();
            }
        }

        [[nodiscard]] constexpr vec<b, T> &operator[](size_t i)
        {
            assert(i < size.x);
            return data[i];
        }
        [[nodiscard]] constexpr vec<b, T> const &operator[](size_t i) const
        {
            assert(i < size.x);
            return data[i];
        }

        [[nodiscard]] constexpr mat<a, b, T> const &operator+() const noexcept
        {
            return *this;
        }

        [[nodiscard]] constexpr mat<a, b, T> operator-() const noexcept
        {
            mat<a, b, T> return_value;
            for (int i = 0; i < size.x; i++)
            {
                return_value.data[i] = -data[i];
            }
            return return_value;
        }

        template <Primitive U>
        constexpr mat<a, b, T> &operator+=(mat<a, b, U> const &other)
        {
            for (int i = 0; i < size.x; i++)
            {
                data[i] += other.data[i];
            }
            return *this;
        }
        template <Primitive U>
        constexpr mat<a, b, T> &operator-=(mat<a, b, U> const &other)
        {
            for (int i = 0; i < size.x; i++)
            {
                data[i] -= other.data[i];
            }
            return *this;
        }

        template <size_t c, Primitive U>
        constexpr mat<a, c, T> &operator*=(mat<b, c, U> const &other)
        {
            return (*this = *this * other);
        }

        template <Primitive U>
        constexpr mat<a, b, T> &operator+=(U const value)
        {
            for (int i = 0; i < size.x; i++)
            {
                data[i] += value;
            }
            return *this;
        }
        template <Primitive U>
        constexpr mat<a, b, T> &operator-=(U const value)
        {
            for (int i = 0; i < size.x; i++)
            {
                data[i] -= value;
            }
            return *this;
        }

        static constexpr ivec2 size{a, b};

        union
        {
            std::array<T, size.x * size.y> arr;
            std::array<vec<size.y, T>, size.x> data;
        };
        static_assert(sizeof(arr) == sizeof(data));
    };
}