#pragma once
#include "mat.hpp"
namespace engine::math
{
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
    template <Primitive T>
    mat<4, 4, T> translate(mat<4, 4, T> const &matrix, vec<3, T> const &vector)
    {
        mat<4, 4, T> return_value(matrix);
        return_value[3] = matrix[0] * vector + matrix[1] * vector + matrix[2] * vector + matrix[3] * vector;
    }
    template <Primitive T>
    mat<4, 4, T> rotate(mat<4, 4, T> const &matrix, T angle, vec<3, T> const &axis)
    {
        }
    template <Primitive T>
    mat<4, 4, T> scale(mat<4, 4, T> const &matrix, vec<3, T> const &vector)
    {
    }
}