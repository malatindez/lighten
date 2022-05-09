#pragma once
#include "vec.hpp"
namespace engine::math
{
    template <size_t rows, size_t columns, Primitive T>
    struct mat;

    template <size_t a, size_t b, size_t c, Primitive T, Primitive U>
    constexpr mat<a, c, T> operator*(mat<a, b, T> const &left, mat<b, c, U> const &right)
    {
        mat<a, c, T> return_value;
        for (size_t i = 0; i < a; i++)
        {
            for (size_t j = 0; j < c; j++)
            {
                return_value[i][j] = 0;
                for (size_t k = 0; k < b; k++)
                {
                    return_value[i][j] += left[i][k] * right[k][j];
                }
            }
        }
        return return_value;
    }

    template <size_t a, size_t b, Primitive T>
    constexpr std::istream &operator>>(std::istream &is, mat<a, b, T> matrix)
    {
        for (int i = 0; i < a; i++)
        {
            for (int j = 0; j < b; j++)
            {
                is >> matrix[i][j];
            }
        }
        return is;
    }
    template <size_t a, size_t b, Primitive T>
    constexpr std::ostream &operator<<(std::ostream &os, mat<a, b, T> matrix)
    {
        for (int i = 0; i < a; i++)
        {
            for (int j = 0; j < b; j++)
            {
                os << matrix[i][j] << " ";
            }
            os << std::endl;
        }
        return os;
    }
}

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

    using mat4 = mat<4, 4, float>;
    using dmat4 = mat<4, 4, double>;
    using imat4 = mat<4, 4, int>;
    using mat4x4 = mat<4, 4, float>;
    using dmat4x4 = mat<4, 4, double>;
    using imat4x4 = mat<4, 4, int>;

    using mat3 = mat<3, 3, float>;
    using dmat3 = mat<3, 3, double>;
    using imat3 = mat<3, 3, int>;
    using mat3x3 = mat<3, 3, float>;
    using dmat3x3 = mat<3, 3, double>;
    using imat3x3 = mat<3, 3, int>;

    using mat2 = mat<2, 2, float>;
    using dmat2 = mat<2, 2, double>;
    using imat2 = mat<2, 2, int>;
    using mat2x2 = mat<2, 2, float>;
    using dmat2x2 = mat<2, 2, double>;
    using imat2x2 = mat<2, 2, int>;

    using mat2x3 = mat<2, 3, float>;
    using dmat2x3 = mat<2, 3, double>;
    using imat2x3 = mat<2, 3, int>;

    using mat3x2 = mat<3, 2, float>;
    using dmat3x2 = mat<3, 2, double>;
    using imat3x2 = mat<3, 2, int>;

    using mat2x4 = mat<2, 4, float>;
    using dmat2x4 = mat<2, 4, double>;
    using imat2x4 = mat<2, 4, int>;

    using mat4x2 = mat<4, 2, float>;
    using dmat4x2 = mat<4, 2, double>;
    using imat4x2 = mat<4, 2, int>;

    using mat3x4 = mat<3, 4, float>;
    using dmat3x4 = mat<3, 4, double>;
    using imat3x4 = mat<3, 4, int>;

    using mat4x3 = mat<4, 3, float>;
    using dmat4x3 = mat<4, 3, double>;
    using imat4x3 = mat<4, 3, int>;

}