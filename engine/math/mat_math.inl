#pragma once
#include "mat_math.hpp"
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
    template <size_t b, size_t c, Primitive T, Primitive U>
    constexpr vec<b, T> operator*(vec<b, T> const &left, mat<b, c, U> const &right)
    {
        vec<b, T> return_value;
        for (size_t j = 0; j < c; j++)
        {
            return_value[j] = 0;
            for (size_t k = 0; k < b; k++)
            {
                return_value[j] += left[k] * right[k][j];
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
    mat<4, 4, T> translate(mat<4, 4, T> const &matrix, vec<3, T> const &vec)
    {
        mat<4, 4, T> return_value;
        return_value[0] = matrix[0];
        return_value[1] = matrix[1];
        return_value[2] = matrix[2];
        return_value[3] = matrix[0] * vec[0] + matrix[1] * vec[1] + matrix[2] * vec[2] + matrix[3];
        return return_value;
    }

    template <Primitive T>
    mat<4, 4, T> rotate(mat<4, 4, T> const &matrix, T angle, vec<3, T> const &vector)
    {
        T const c = std::cos(angle);
        T const s = std::sin(angle);
        vec<3, T> axis = normalize(vector);
        vec<3, T> temp = (T(1) - c) * axis;
        mat<4, 4, T> rotate;

        rotate[0][0] = c + temp[0] * axis[0];
        rotate[0][1] = temp[0] * axis[1] + s * axis[2];
        rotate[0][2] = temp[0] * axis[2] - s * axis[1];

        rotate[1][0] = temp[1] * axis[0] - s * axis[2];
        rotate[1][1] = c + temp[1] * axis[1];
        rotate[1][2] = temp[1] * axis[2] + s * axis[0];

        rotate[2][0] = temp[2] * axis[0] + s * axis[1];
        rotate[2][1] = temp[2] * axis[1] - s * axis[0];
        rotate[2][2] = c + temp[2] * axis[2];

        mat<4, 4, T> result(1);
        result[0] = matrix[0] * rotate[0][0] + matrix[1] * rotate[0][1] + matrix[2] * rotate[0][2];
        result[1] = matrix[0] * rotate[1][0] + matrix[1] * rotate[1][1] + matrix[2] * rotate[1][2];
        result[2] = matrix[0] * rotate[2][0] + matrix[1] * rotate[2][1] + matrix[2] * rotate[2][2];
        result[3] = matrix[3];
        return result;
    }

    template <Primitive T>
    mat<4, 4, T> scale(mat<4, 4, T> const &matrix, vec<3, T> const &scale)
    {
        mat<4, 4, T> return_value;
        return_value[0] = matrix[0] * scale[0];
        return_value[1] = matrix[1] * scale[1];
        return_value[2] = matrix[2] * scale[2];
        return_value[3] = matrix[3];
        return return_value;
    }

    template <size_t a, size_t b, Primitive T>
    mat<b, a, T> transpose(mat<a, b, T> const &matrix)
    {
        mat<b, a, T> return_value;
        for (int i = 0; i < a; i++)
        {
            for (int j = 0; j < b; j++)
            {
                return_value[j][i] = matrix[i][j];
            }
        }
        return return_value;
    }
}