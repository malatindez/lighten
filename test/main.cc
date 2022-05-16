
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
    math::imat4 matrix(1);
    matrix = math::imat4(12, 34, 12, 5, 102, 3, 451, 2, 52, 5, 12, 5, 2, 1, 2, 415);
    math::mat inv_ = math::inv(math::mat(matrix));
    std::cout << matrix << std::endl;
    std::cout << math::imat4(math::adj(math::mat(matrix))) << std::endl;
    std::cout << inv_ << std::endl;
    math::mat a = math::mat(matrix) * inv_;
    std::cout << a << std::endl;
    std::cout << math::imat4(a) << std::endl;
}