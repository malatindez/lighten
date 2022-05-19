
#include <iostream>

#include "math.hpp"
#include "pch.hpp"

namespace math = engine::math;
template <size_t a, size_t b, Primitive T>
void random_fill(math::mat<a, b, T> &matrix)
{
    for (int i = 0; i < matrix.size.x; i++)
    {
        for (int j = 0; j < matrix.size.y; j++)
        {
            matrix[i][j] = rand() % 2;
        }
    }
}
constexpr float pi = 3.141592653589897932384626433;

int main()
{
    math::mat4 matrix(1);
    matrix = math::mat4(12, 34, 12, 5, 102, 3, 451, 2, 52, 5, 12, 5, 2, 1, 2, 415);
    std::cout << matrix << std::endl;
    matrix = math::invert_orthogonal(matrix);
    std::cout << matrix << std::endl;
    matrix = math::inverse(matrix);
    std::cout << matrix << std::endl;
}